#include "server_manager.hpp"

namespace server {

ServerManager::ServerManager(const Configuration& configuration)
	: configuration_(configuration)
	, sockets_(std::vector<server::TcpSocket>())
	, highest_sd_(-1)
	, is_running(false) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&master_write_fds_);
	FD_ZERO(&read_fds_);
	FD_ZERO(&write_fds_);
	timeout_.tv_sec = 30;
	timeout_.tv_usec = 0;
}

ServerManager::~ServerManager() {}

ServerManager::ServerManager(const ServerManager& other)
	: configuration_(other.configuration_)
	, sockets_(other.sockets_)
	, master_read_fds_(other.master_read_fds_)
	, master_write_fds_(other.master_write_fds_)
	, read_fds_(other.read_fds_)
	, write_fds_(other.write_fds_)
	, highest_sd_(other.highest_sd_)
	, is_running(other.is_running)
	, timeout_(other.timeout_) {}

ServerManager& ServerManager::operator=(const ServerManager& other) {
	if (this != &other) {
		sockets_ = other.sockets_;
		master_read_fds_ = other.master_read_fds_;
		master_write_fds_ = other.master_write_fds_;
		read_fds_ = other.read_fds_;
		write_fds_ = other.write_fds_;
		highest_sd_ = other.highest_sd_;
		is_running = other.is_running;
		timeout_ = other.timeout_;
	}
	return *this;
}

int ServerManager::runServer() {
	if (setupServerSockets() < 0) {
		std::cerr << "setupServerSocket() failed" << std::endl;
		return -1;
	}
	setupSelectReadFds();
	return monitorSocketEvents();
}

int ServerManager::setupServerSockets() {
	std::vector<ServerDirective> server_configurations = configuration_.getServers();
	for (size_t i = 0; i < server_configurations.size(); ++i) {
		sockets_.push_back(server::TcpSocket(
			server_configurations[i].getIpAddress(), server_configurations[i].getPort()));
	}

	for (size_t i = 0; i < sockets_.size();) {
		if (sockets_[i].prepareSocketForListening() < 0) {
			sockets_.erase(sockets_.begin() + i);
		} else {
			++i;
		}
	}

	for (size_t i = 0; i < sockets_.size();) {
		if (sockets_[i].startListening() < 0) {
			sockets_.erase(sockets_.begin() + i);
		} else {
			++i;
		}
	}

	if (sockets_.empty()) {
		std::cerr << "Initialization of all addresses failed" << std::endl;
		return -1;
	}
	return 0;
}

int ServerManager::setupSelectReadFds() {
	FD_ZERO(&master_read_fds_);
	for (size_t i = 0; i < sockets_.size(); ++i) {
		if (sockets_[i].getListenSd() > highest_sd_) {
			highest_sd_ = sockets_[i].getListenSd();
		}
		FD_SET(sockets_[i].getListenSd(), &master_read_fds_);
	}
	return 0;
}

int ServerManager::monitorSocketEvents() {
	is_running = true;
	while (is_running) {
#ifdef FD_COPY
		std::memset(&read_fds_, 0, sizeof(read_fds_));
		std::memset(&write_fds_, 0, sizeof(write_fds_));
		FD_COPY(&master_read_fds_, &read_fds_);
		FD_COPY(&master_write_fds_, &write_fds_);
#else
		std::memset(&read_fds_, 0, sizeof(read_fds_));
		std::memset(&write_fds_, 0, sizeof(write_fds_));
		std::memcpy(&read_fds_, &master_read_fds_, sizeof(master_read_fds_));
		std::memcpy(&write_fds_, &master_write_fds_, sizeof(master_write_fds_));
#endif
		std::cout << "Waiting on select()!" << std::endl;
		int select_result = select(highest_sd_ + 1, &read_fds_, &write_fds_, NULL, &timeout_);

		if (select_result < 0) {
			std::cerr << "select() failed: " << strerror(errno) << std::endl;
			break;
		}

		if (select_result == 0) {
			std::cout << "select() timed out. continue" << std::endl;
			timeout_.tv_sec = 30;
			timeout_.tv_usec = 0;
			continue;
		}

		if (dispatchSocketEvents(select_result) < 0) {
			std::cerr << "dispatchSocketEvents() failed" << std::endl;
			break;
		}
	}
	return 0;
}

void ServerManager::handleCgiProcess(ClientSession& client_session) {
	cgi::Cgi& cgi = client_session.getCgi();
	const HttpRequest& request = client_session.getRequest();

	if (cgi.setup() < 0 || cgi.execute() < 0) {
		createErrorResponse(client_session.getResponse(),
			http_status_code::INTERNAL_SERVER_ERROR,
			client_session.findLocation());
		client_session.getResponse().concatenateComponents();
		client_session.setStatus(SENDING_RESPONSE);
		return;
	}
	cgi::CgiResponse& cgi_response = client_session.getCgiResponse();
	cgi_response.setSocketFd(0, cgi.getSocketFd(0));
	cgi_response.setSocketFd(1, cgi.getSocketFd(1));
	cgi_response.setPid(cgi.getPid());
	cgi_response.setStage(cgi::HEADERS_SENT);

	if (request.getHttpMethod() == http_method::POST) {
		client_session.setStatus(CGI_BODY_SENDING);
		setWriteFd(cgi.getSocketFd(0));
		cgi_socket_pairs_.insert(std::make_pair(cgi.getSocketFd(0), client_session.getSd()));
	} else {
		client_session.setStatus(CGI_RECEIVEING);
		setReadFd(cgi.getSocketFd(0));
		cgi_socket_pairs_.insert(std::make_pair(cgi.getSocketFd(0), client_session.getSd()));
	}
}

int ServerManager::dispatchSocketEvents(int ready_sds) {

	for (int sd = 0; sd <= highest_sd_ && ready_sds > 0; ++sd) {
		if (FD_ISSET(sd, &read_fds_)) {
			if (isListeningSocket(sd)) {
				if (acceptIncomingConnection(sd) < 0) {
					is_running = false;
					return -1;
				}
			} else {
				int client_sd = sd;
				if (cgi_socket_pairs_.find(sd) != cgi_socket_pairs_.end()) {
					client_sd = cgi_socket_pairs_.find(sd)->second;
				}
				ClientSession& client_session = getClientSession(client_sd);
				if (client_session.getStatus() != CGI_RECEIVEING) {
					if (receiveAndParseHttpRequest(client_session) < 0) {
						is_running = false;
						std::cout << "receiveAndParseHttpRequest() failed" << std::endl;
						return -1;
					}
					if (client_session.getStatus() == CLOSED) {
						unregisterClientSession(client_session);
						--ready_sds;
						continue;
					}
					if (client_session.getStatus() == EVALUATING_RESPONSE_TYPE) {
						setClientResponseStage(client_session);
					}
					if (client_session.getStatus() == RESPONSE_PREPARING) {
						handle_request::handleRequest(client_session);
					} else if (client_session.getStatus() == CGI_PREPARING) {
						handleCgiProcess(client_session);
					}
					if (client_session.getStatus() == SENDING_RESPONSE) {
						setWriteFd(sd);
					}

				} else {
					if (client_session.getCgiResponse().readCgiReponse() < 0) {
						createErrorResponse(client_session.getResponse(),
							http_status_code::INTERNAL_SERVER_ERROR,
							client_session.findLocation());
						client_session.getResponse().concatenateComponents();
						cgi_socket_pairs_.erase(sd);
						int client_sd = client_session.getCgi().getSocketFd(0);
						clearFds(client_sd);
						close(client_session.getCgi().getSocketFd(0));
						setWriteFd(client_session.getSd());
						client_session.setStatus(SENDING_RESPONSE);
					} else if (client_session.getCgi().getStatus() == cgi::EXECUTED &&
							   client_session.getCgiResponse().getStage() == cgi::COMPLETE) {
						handle_cgi_response::handleCgiResponse(client_session);
						cgi_socket_pairs_.erase(sd);
						int client_sd = client_session.getCgi().getSocketFd(0);
						clearFds(client_sd);

						close(client_session.getCgi().getSocketFd(0));
						setWriteFd(client_session.getSd());
						client_session.setStatus(SENDING_RESPONSE);
					}
				}
			}
			--ready_sds;
		} else if (FD_ISSET(sd, &write_fds_)) {
			int client_sd = sd;
			if (cgi_socket_pairs_.find(sd) != cgi_socket_pairs_.end()) {
				client_sd = cgi_socket_pairs_.find(sd)->second;
			}
			ClientSession& client_session = getClientSession(client_sd);
			if (client_session.getStatus() == CGI_BODY_SENDING) {
				sendCgiBody(client_session);
			}
			if (client_session.getStatus() == SENDING_RESPONSE) {
				sendResponse(client_session);
			}
			if (client_session.getStatus() == CLOSED) {
				unregisterClientSession(client_session);
				--ready_sds;
				continue;
			} else if (client_session.getStatus() == SESSION_COMPLETE) {
				clearFds(client_session.getSd());
				client_session.sessionCleanup();
			}
			--ready_sds;
		}
	}
	return 0;
}

int ServerManager::sendCgiBody(ClientSession& client_session) {
	int client_sd = client_session.getCgi().getSocketFd(0);
	std::string body = client_session.getRequest().getBody();
	char buffer[BUFFER_SIZE];
	std::memset(buffer, '\0', sizeof(buffer));
	std::memcpy(buffer, body.c_str(), body.length());
	int send_result = ::send(client_sd, buffer, sizeof(buffer), 0);
	if (send_result < 0) {
		std::cerr << "send() failed: " << strerror(errno) << std::endl;
		closeClientSession(client_session);
		cgi_socket_pairs_.erase(client_session.getCgi().getSocketFd(0));
		close(client_session.getCgi().getSocketFd(0));
		clearFds(client_session.getCgi().getSocketFd(0));
		return -1;
	}
	if (send_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		closeClientSession(client_session);
		cgi_socket_pairs_.erase(client_session.getCgi().getSocketFd(0));
		close(client_session.getCgi().getSocketFd(0));
		clearFds(client_session.getCgi().getSocketFd(0));
		return -1;
	}
	std::cout << "\033[31m"
			  << "  CGI BODY SENDING: complete"
			  << "\033[0m" << std::endl;
	clearFds(client_session.getCgi().getSocketFd(0));
	setReadFd(client_session.getCgi().getSocketFd(0));
	client_session.setStatus(CGI_RECEIVEING);
	// close(client_session.getCgi().getSocketFd(1));
	return 0;
}

void ServerManager::setClientResponseStage(ClientSession& session) {
	const ServerDirective& server_directive = session.getServerDirective();
	const HttpRequest& request = session.getRequest();
	const LocationDirective& location_directive =
		server_directive.findLocation(request.getUriPath());
	std::string file_extension = Utils::extructUriExtension(request.getUriPath());
	std::string file_path = location_directive.getRoot() + request.getUriPath();

	std::string::size_type pos = file_extension.find('/');
	if (pos != std::string::npos) {
		std::string path_info = file_extension.substr(pos);
		file_extension = file_extension.erase(pos, file_extension.size());
		file_path = location_directive.getRoot() + path_info + request.getUriPath();
		file_path = file_path.erase(file_path.size() - path_info.size(), file_path.size());
	}
	if (location_directive.isCgiEnabled() && location_directive.isCgiExtension(file_extension) &&
		Utils::fileExists(file_path)) {
		std::map<std::string, std::string> meta_variables;
		server_cgi_utils::createCgiMetaVariables(meta_variables,
			session.getRequest(),
			session.getClientAddress(),
			session.getServerAddress(),
			location_directive.getRoot());
		cgi::Cgi* cgi = new (std::nothrow) cgi::Cgi(meta_variables);
		if (!cgi) {
			std::cerr << "new (std::nothrow) Cgi() failed" << std::endl;
			createErrorResponse(session.getResponse(),
				http_status_code::INTERNAL_SERVER_ERROR,
				session.findLocation());
			session.setStatus(SENDING_RESPONSE);
			return;
		}
		session.setCgi(cgi);
		session.setStatus(CGI_PREPARING);
	} else {
		session.setStatus(RESPONSE_PREPARING);
	}
	std::cout << "session.getStatus(): " << session.getStatus() << std::endl;
	return;
}

bool ServerManager::isListeningSocket(int sd) {
	for (size_t i = 0; i < sockets_.size(); ++i) {
		if (sd == sockets_[i].getListenSd()) {
			return true;
		}
	}
	return false;
}

int ServerManager::acceptIncomingConnection(int listen_sd) {

	int client_sd;
	std::cout << "  Listening socket is readable" << std::endl;
	do {
		sockaddr_in client_socket_address;
		socklen_t client_socket_address_len = sizeof(client_socket_address);
		std::memset(&client_socket_address, 0, sizeof(client_socket_address));
		client_sd =
			accept(listen_sd, (sockaddr*)&client_socket_address, &client_socket_address_len);
		if (client_sd < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				return -1;
			}
			break;
		}

		if (setNonBlocking(client_sd) < 0) {
			std::cerr << "setNonBlocking() failed" << std::endl;
			return -1;
		}

		sockaddr_in connected_server_address;
		socklen_t server_socket_address_len = sizeof(connected_server_address);

		if (getsockname(client_sd,
				(struct sockaddr*)&connected_server_address,
				&server_socket_address_len) == -1) {
			std::cerr << "getsockname(): " << strerror(errno) << std::endl;
			return -1;
		}

		registerClientSession(client_sd, client_socket_address, connected_server_address);

		std::cout << "  New incoming connection -  " << client_sd << std::endl;
		FD_SET(client_sd, &master_read_fds_);
		if (client_sd > highest_sd_)
			highest_sd_ = client_sd;

	} while (client_sd != -1);
	return 0;
}

int ServerManager::setNonBlocking(int sd) {
	int flags = fcntl(sd, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "fcntl() get flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	flags |= O_NONBLOCK;
	flags = fcntl(sd, F_SETFL, flags);
	if (flags < 0) {
		std::cerr << "fcntl() set flags failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

void ServerManager::registerClientSession(int sd,
	sockaddr_in client_address,
	sockaddr_in server_address) {
	if (active_client_sessions_.find(sd) != active_client_sessions_.end()) {
		return;
	}
	std::string server_ip_address = inet_ntoa(server_address.sin_addr);
	std::ostringstream port_stream;
	port_stream << ntohs(server_address.sin_port);
	std::string client_port = port_stream.str();
	ServerDirective const& server_directive =
		configuration_.getServerDirective(server_ip_address, client_port);
	active_client_sessions_.insert(
		std::make_pair(sd, ClientSession(sd, client_address, server_address, server_directive)));
}

int ServerManager::receiveAndParseHttpRequest(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	char recv_buffer[BUFFER_SIZE];
	std::memset(recv_buffer, '\0', sizeof(recv_buffer));

	int recv_result = recv(client_sd, recv_buffer, sizeof(recv_buffer) - 1, 0);
	if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		closeClientSession(client_session);
		return -1;
	}
	if (recv_result == 0) {
		closeClientSession(client_session);
		return 0;
	}

	HttpRequest& request = client_session.getRequest();
	HttpRequestParser::parse(request, recv_buffer);
	client_session.setSessionStatusFromHttpRequest();

	return 0;
}

int ServerManager::setWriteFd(int sd) {
	FD_SET(sd, &master_write_fds_);
	if (sd > highest_sd_)
		highest_sd_ = sd;
	return 0;
}

int ServerManager::setReadFd(int sd) {
	FD_SET(sd, &master_read_fds_);
	if (sd > highest_sd_)
		highest_sd_ = sd;
	return 0;
}

int ServerManager::sendResponse(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	char send_buffer[BUFFER_SIZE];
	std::memset(send_buffer, '\0', sizeof(send_buffer));
	client_session.getResponse().getStreamBuffer(send_buffer, BUFFER_SIZE - 1);
	int send_result = send(client_sd, send_buffer, sizeof(send_buffer), 0);
	std::cout << "send_buffer: " << send_buffer << std::endl;
	if (send_result < 0) {
		std::cerr << "send() failed: " << strerror(errno) << std::endl;
		closeClientSession(client_session);
		return -1;
	}
	if (send_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		closeClientSession(client_session);
		return -1;
	}
	if (client_session.getStatus() == SENDING_RESPONSE) {
		if (client_session.getResponse().getStatus() == http_response_status::FINISHED) {
			client_session.setStatus(SESSION_COMPLETE);
		}
		std::cout << "\033[31m"
				  << "  SENDING: complete"
				  << "\033[0m" << std::endl;
	}
	return 0;
}

void ServerManager::closeClientSession(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	// FD_CLR(client_sd, &master_read_fds_);
	// FD_CLR(client_sd, &master_write_fds_);
	clearFds(client_sd);
	// if (client_sd == highest_sd_) {
	// 	while (!FD_ISSET(highest_sd_, &master_read_fds_))
	// 		--highest_sd_;
	// }
	close(client_sd);
	client_session.setStatus(CLOSED);
	std::cout << "  						Connection closed - " << client_sd << std::endl;
}

int ServerManager::unregisterClientSession(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	std::cout << "  Connection closed - " << client_sd << std::endl;
	active_client_sessions_.erase(client_sd);
	return 0;
}

ClientSession& ServerManager::getClientSession(int const sd) {
	std::map<int, ClientSession>::iterator it = active_client_sessions_.find(sd);
	if (it == active_client_sessions_.end()) {
		std::cerr << "map find() err: " << __FILE__ << " : " << __LINE__ << std::endl;
		static ServerDirective dummy_server_directive;
		static ClientSession dummy_client_session(
			-1, sockaddr_in(), sockaddr_in(), dummy_server_directive, CLOSED);
		return dummy_client_session;
	}
	return it->second;
}

Configuration const& ServerManager::getConfiguration() const {
	return configuration_;
}

std::vector<server::TcpSocket> const& ServerManager::getSockets() const {
	return sockets_;
}

std::map<int, ClientSession> const& ServerManager::getActiveClientSessions() const {
	return active_client_sessions_;
}

fd_set const& ServerManager::getMasterReadFds() const {
	return master_read_fds_;
}

fd_set const& ServerManager::getMasterWriteFds() const {
	return master_write_fds_;
}

fd_set const& ServerManager::getReadFds() const {
	return read_fds_;
}

fd_set const& ServerManager::getWriteFds() const {
	return write_fds_;
}

int ServerManager::getHighestSd() const {
	return highest_sd_;
}

bool ServerManager::getIsRunning() const {
	return is_running;
}

void ServerManager::clearFds(int sd) {
	if (FD_ISSET(sd, &master_read_fds_)) {
		FD_CLR(sd, &master_read_fds_);
	}
	if (FD_ISSET(sd, &master_write_fds_)) {
		FD_CLR(sd, &master_write_fds_);
	}
	if (highest_sd_ == sd) {
		while (!FD_ISSET(highest_sd_, &master_read_fds_) &&
			   !FD_ISSET(highest_sd_, &master_write_fds_)) {
			--highest_sd_;
		}
	}
}

struct timeval const& ServerManager::getTimeout() const {
	return timeout_;
}

std::ostream& operator<<(std::ostream& out, const ServerManager& server_manager) {
	out << "ServerManager: " << std::endl;
	out << "  configuration_: " << server_manager.getConfiguration() << std::endl;
	out << "  sockets_: " << std::endl;
	for (size_t i = 0; i < server_manager.getSockets().size(); ++i) {
		out << "    " << server_manager.getSockets()[i] << std::endl;
	}
	out << "  active_client_sessions_: " << std::endl;
	for (std::map<int, ClientSession>::const_iterator it =
			 server_manager.getActiveClientSessions().begin();
		 it != server_manager.getActiveClientSessions().end();
		 ++it) {
		out << "    " << it->second << std::endl;
	}
	out << "  master_read_fds_: " << std::endl;
	for (int i = 0; i <= server_manager.getHighestSd(); ++i) {
		if (FD_ISSET(i, &server_manager.getMasterReadFds())) {
			out << "    " << i << std::endl;
		}
	}
	out << "  master_write_fds_: " << std::endl;
	for (int i = 0; i <= server_manager.getHighestSd(); ++i) {
		if (FD_ISSET(i, &server_manager.getMasterWriteFds())) {
			out << "    " << i << std::endl;
		}
	}
	out << "  read_fds_: " << std::endl;
	for (int i = 0; i <= server_manager.getHighestSd(); ++i) {
		if (FD_ISSET(i, &server_manager.getReadFds())) {
			out << "    " << i << std::endl;
		}
	}
	out << "  write_fds_: " << std::endl;
	for (int i = 0; i <= server_manager.getHighestSd(); ++i) {
		if (FD_ISSET(i, &server_manager.getWriteFds())) {
			out << "    " << i << std::endl;
		}
	}
	out << "  highest_sd_: " << server_manager.getHighestSd() << std::endl;
	out << "  is_running: " << server_manager.getIsRunning() << std::endl;
	out << "  timeout_: " << server_manager.getTimeout().tv_sec << std::endl;
	return out;
}

} // namespace server
