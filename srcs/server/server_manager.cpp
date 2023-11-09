#include "server_manager.hpp"
#include "parse_http_request.hpp"

namespace server {

ServerManager::ServerManager(Configuration& configuration)
	: configuration_(configuration)
	, socket_(std::vector<server::Socket>())
	, highest_socket_descriptor_(-1)
	, server_is_running_(false) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&read_fds__);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

ServerManager::~ServerManager() {}

ServerManager::ServerManager(const ServerManager& other)
	: configuration_(other.configuration_)
	, socket_(other.socket_)
	, highest_socket_descriptor_(other.highest_socket_descriptor_)
	, timeout_(other.timeout_)
	, master_read_fds_(other.master_read_fds_)
	, read_fds__(other.read_fds__)
	, server_is_running_(other.server_is_running_) {}

ServerManager& ServerManager::operator=(const ServerManager& other) {
	if (this != &other) {
		configuration_ = other.configuration_;
		socket_ = other.socket_;
		highest_socket_descriptor_ = other.highest_socket_descriptor_;
		timeout_ = other.timeout_;
		master_read_fds_ = other.master_read_fds_;
		read_fds__ = other.read_fds__;
		server_is_running_ = other.server_is_running_;
	}
	return *this;
}

int ServerManager::setUpServerSockets() {
	std::vector<ServerDirective> servers = configuration_.getServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		socket_.push_back(server::Socket(servers[i].getIpAddress(), servers[i].getPort()));
	}

	for (size_t i = 0; i < socket_.size();) {
		if (socket_[i].initialize() < 0) {
			socket_.erase(socket_.begin() + i);
		} else {
			++i;
		}
	}

	if (socket_.empty()) {
		std::cerr << "Initialization of all addresses failed" << std::endl;
		return -1;
	}
	return 0;
}

int ServerManager::acceptIncomingConnection(int listen_sd) {

	int client_socket_descriptor;
	std::cout << "  Listening socket is readable" << std::endl;
	do {
		sockaddr_in client_socket_address;
		socklen_t client_socket_address_len = sizeof(client_socket_address);
		memset(&client_socket_address, 0, sizeof(client_socket_address));
		client_socket_descriptor =
			accept(listen_sd, (sockaddr*)&client_socket_address, &client_socket_address_len);
		if (client_socket_descriptor < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				return -1;
			}
			break;
		}
		sockaddr_in connected_server_address;
		socklen_t server_socket_address_len = sizeof(connected_server_address);

		if (getsockname(client_socket_descriptor,
				(struct sockaddr*)&connected_server_address,
				&server_socket_address_len) == -1) {
			std::cerr << "getsockname(): " << strerror(errno) << std::endl;
			return -1;
		}

		http_request_parse_.addAcceptClientInfo(
			client_socket_descriptor, client_socket_address, connected_server_address);

		std::cout << "  New incoming connection -  " << client_socket_descriptor << std::endl;
		FD_SET(client_socket_descriptor, &master_read_fds_);
		if (client_socket_descriptor > highest_socket_descriptor_)
			highest_socket_descriptor_ = client_socket_descriptor;

	} while (client_socket_descriptor != -1);
	return 0;
}

int ServerManager::disconnect(int sd) {
	close(sd);
	FD_CLR(sd, &master_read_fds_);
	if (sd == highest_socket_descriptor_) {
		while (!FD_ISSET(highest_socket_descriptor_, &master_read_fds_))
			--highest_socket_descriptor_;
	}
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

bool ServerManager::isListeningSocket(int sd) {
	for (size_t i = 0; i < socket_.size(); ++i) {
		if (sd == socket_[i].getListenSd()) {
			return true;
		}
	}
	return false;
}

int ServerManager::dispatchSocketEvents(int readyDescriptors) {

	for (int descriptor = 0; descriptor <= highest_socket_descriptor_ && readyDescriptors > 0;
		 ++descriptor) {
		if (FD_ISSET(descriptor, &read_fds__)) {
			if (isListeningSocket(descriptor)) {
				if (acceptIncomingConnection(descriptor) < 0) {
					server_is_running_ = false;
					return -1;
				}
			} else {
				if (receiveAndParseHttpRequest(descriptor) < 0) {
					server_is_running_ = false;
					return -1;
				}
			}
			--readyDescriptors;
		}
	}
	return 0;
}

int ServerManager::monitorSocketEvents() {
	server_is_running_ = true;
	while (server_is_running_) {
		std::memcpy(&read_fds__, &master_read_fds_, sizeof(master_read_fds_));

		std::cout << "Waiting on select()!" << std::endl;
		int select_result =
			select(highest_socket_descriptor_ + 1, &read_fds__, NULL, NULL, &timeout_);

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

int ServerManager::setupSelectReadFds() {
	FD_ZERO(&master_read_fds_);
	for (size_t i = 0; i < socket_.size(); ++i) {
		if (socket_[i].getListenSd() > highest_socket_descriptor_) {
			highest_socket_descriptor_ = socket_[i].getListenSd();
		}
		FD_SET(socket_[i].getListenSd(), &master_read_fds_);
	}
	return 0;
}

int ServerManager::runServer() {
	if (setUpServerSockets() < 0) {
		std::cerr << "setupServerSocket() failed" << std::endl;
		return -1;
	}
	setupSelectReadFds();
	return monitorSocketEvents();
}
}
