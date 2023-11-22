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
	timeout_.tv_sec = 10;
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
			std::cout << "select() timed out.  End program." << std::endl;
			timeout_.tv_sec = 10;
			timeout_.tv_usec = 0;
			continue;
		}
		if (dispatchSocketEvents(select_result) < 0) {
			continue;
		}
	}
	return 0;
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
				ClientSession& client_session = getClientSession(sd);
				if (receiveAndParseHttpRequest(client_session) < 0) {
					is_running = false;
					return -1;
				}
			}
			--ready_sds;
		} else if (FD_ISSET(sd, &write_fds_)) {
			sendResponse(sd);
			--ready_sds;
		}
	}
	return 0;
}

void ServerManager::determineRequestType(int sd) {
	// この関数は,リファクタするので,今は仮実装
	(void)sd;
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

void ServerManager::registerClientSession(int sd,
	sockaddr_in client_address,
	sockaddr_in server_address) {
	if (client_session_.find(sd) != client_session_.end()) {
		return;
	}
	client_session_.insert(std::make_pair(sd, ClientSession(sd, client_address, server_address)));
}

int ServerManager::receiveAndParseHttpRequest(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	char recv_buffer[BUFFER_SIZE];
	std::memset(recv_buffer, '\0', sizeof(recv_buffer));

	int recv_result = recv(client_sd, recv_buffer, sizeof(recv_buffer) - 1, 0);
	if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		unregisterClientSession(client_session);
		return -1;
	}
	if (recv_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		unregisterClientSession(client_session);
		return 0;
	}

	HttpRequest& request = client_session.getRequest();
	(void)request;

	// ここでリクエストをパースする

	// リクエストのパース終了
	client_session.setSessionStatusFromHttpRequest();

	return 0;
}

int ServerManager::setWriteFd(int sd) {
	FD_SET(sd, &master_write_fds_);
	return 0;
}

int ServerManager::sendResponse(int sd) {
	char send_buffer[BUFFER_SIZE];
	std::memset(send_buffer, '\0', sizeof(send_buffer));
	std::string buffer = "HTTP/1.1 200 OK\r\n"
						 "Date: Wed, 09 Nov 2023 12:00:00 GMT\r\n"
						 "Server: MyServer\r\n"
						 "Content-Type: text/html; charset=UTF-8\r\n"
						 "Content-Length: 97\r\n"
						 "\r\n"
						 "<html>\r\n"
						 "<head>\r\n"
						 "<title>Simple Page</title>\r\n"
						 "</head>\r\n"
						 "<body>\r\n"
						 "<h1>Hello, World!</h1>\r\n"
						 "</body>\r\n"
						 "</html>\r\n";
	std::memcpy(send_buffer, buffer.c_str(), buffer.length());
	int send_result = ::send(sd, send_buffer, sizeof(send_buffer), 0);
	(void)send_result;
	// if (send_result < 0) {
	// 	std::cerr << "send() failed: " << strerror(errno) << std::endl;
	// 	disconnect(sd);
	// 	return -1;
	// }
	// if (send_result == 0) {
	// 	std::cout << "  Connection closed" << std::endl;
	// 	disconnect(sd);
	// 	return -1;
	// }
	// server_status_[sd] = COMPLETE;
	// // if (server_status_[sd] == COMPLETE) {
	// 	requestCleanup(sd);
	// 	std::cout << "  Connection Cleanup" << std::endl;
	// }
	return 0;
}

int ServerManager::requestCleanup(int sd) {
	http_request_parser_.httpRequestCleanup(sd);
	FD_CLR(sd, &master_write_fds_);
	// server_status_[sd] = RECEIVING_REQUEST;
	return 0;
}

int ServerManager::unregisterClientSession(ClientSession& client_session) {
	int client_sd = client_session.getSd();
	FD_CLR(client_sd, &master_read_fds_);
	FD_CLR(client_sd, &master_write_fds_);
	if (client_sd == highest_sd_) {
		while (!FD_ISSET(highest_sd_, &master_read_fds_))
			--highest_sd_;
	}
	close(client_sd);
	client_session_.erase(client_sd);
	std::cout << "  Connection closed - " << client_sd << std::endl;
	return 0;
}

ClientSession& ServerManager::getClientSession(int const sd) {
	return client_session_.find(sd)->second;
}

} // namespace server
