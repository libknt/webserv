#include "server_manager.hpp"
#include "parse_http_request.hpp"

namespace server {

ServerManager::ServerManager(const Configuration& configuration)
	: configuration_(configuration)
	, sockets_(std::vector<server::TcpSocket>())
	, highest_sd_(-1)
	, is_running(false) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&read_fds__);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

ServerManager::~ServerManager() {}

ServerManager::ServerManager(const ServerManager& other)
	: configuration_(other.configuration_)
	, sockets_(other.sockets_)
	, master_read_fds_(other.master_read_fds_)
	, read_fds__(other.read_fds__)
	, highest_sd_(other.highest_sd_)
	, is_running(other.is_running)
	, timeout_(other.timeout_) {}

ServerManager& ServerManager::operator=(const ServerManager& other) {
	if (this != &other) {
		sockets_ = other.sockets_;
		highest_sd_ = other.highest_sd_;
		timeout_ = other.timeout_;
		master_read_fds_ = other.master_read_fds_;
		read_fds__ = other.read_fds__;
		is_running = other.is_running;
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
	std::vector<ServerDirective> server_configurations = configuration_.getServerConfigurations();
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
		std::memcpy(&read_fds__, &master_read_fds_, sizeof(master_read_fds_));

		std::cout << "Waiting on select()!" << std::endl;
		int select_result = select(highest_sd_ + 1, &read_fds__, NULL, NULL, &timeout_);

		if (select_result < 0) {
			std::cerr << "select() failed: " << strerror(errno) << std::endl;
			break;
		}

		if (select_result == 0) {
			std::cout << "select() timed out.  End program." << std::endl;
			continue;
		}
		if (dispatchSocketEvents(select_result) < 0) {
			continue;
		}
	}
	return 0;
}

int ServerManager::dispatchSocketEvents(int readyDescriptors) {

	for (int descriptor = 0; descriptor <= highest_sd_ && readyDescriptors > 0; ++descriptor) {
		if (FD_ISSET(descriptor, &read_fds__)) {
			if (isListeningSocket(descriptor)) {
				if (acceptIncomingConnection(descriptor) < 0) {
					is_running = false;
					return -1;
				}
			} else {
				if (receiveAndParseHttpRequest(descriptor) < 0) {
					is_running = false;
					return -1;
				}
			}
			--readyDescriptors;
		}
	}
	return 0;
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

		http_request_parse_.addAcceptClientInfo(
			client_sd, client_socket_address, connected_server_address);

		std::cout << "  New incoming connection -  " << client_sd << std::endl;
		FD_SET(client_sd, &master_read_fds_);
		if (client_sd > highest_sd_)
			highest_sd_ = client_sd;

	} while (client_sd != -1);
	return 0;
}

int ServerManager::receiveAndParseHttpRequest(int sd) {
	char recv_buffer[BUFFER_SIZE];
	std::memset(recv_buffer, '\0', sizeof(recv_buffer));

	int recv_result = recv(sd, recv_buffer, sizeof(recv_buffer), 0);
	if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		disconnect(sd);
		return -1;
	}
	if (recv_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		disconnect(sd);
		return 0;
	}
	http_request_parse_.handleBuffer(sd, recv_buffer);

	return 0;
}

int ServerManager::disconnect(int sd) {
	close(sd);
	FD_CLR(sd, &master_read_fds_);
	if (sd == highest_sd_) {
		while (!FD_ISSET(highest_sd_, &master_read_fds_))
			--highest_sd_;
	}
	return 0;
}

}
