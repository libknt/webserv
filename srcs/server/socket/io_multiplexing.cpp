#include "io_multiplexing.hpp"
#include "parse_http_request.hpp"

namespace server {

IoMultiplexing::IoMultiplexing(Configuration& configuration)
	: configuration_(configuration)
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, highest_socket_descriptor_(-1)
	, max_clients_(-1)
	, server_is_running_(false) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&read_fds__);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

IoMultiplexing::~IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(const IoMultiplexing& other)
	: configuration_(other.configuration_)
	, socket_(other.socket_)
	, activity_times_(other.activity_times_)
	, highest_socket_descriptor_(other.highest_socket_descriptor_)
	, max_clients_(other.max_clients_)
	, timeout_(other.timeout_)
	, master_read_fds_(other.master_read_fds_)
	, read_fds__(other.read_fds__)
	, server_is_running_(other.server_is_running_) {}

IoMultiplexing& IoMultiplexing::operator=(const IoMultiplexing& other) {
	if (this != &other) {
		configuration_ = other.configuration_;
		socket_ = other.socket_;
		activity_times_ = other.activity_times_;
		highest_socket_descriptor_ = other.highest_socket_descriptor_;
		max_clients_ = other.max_clients_;
		timeout_ = other.timeout_;
		master_read_fds_ = other.master_read_fds_;
		read_fds__ = other.read_fds__;
		server_is_running_ = other.server_is_running_;
	}
	return *this;
}

int IoMultiplexing::setUpServerSockets() {
	std::vector<ServerDirective> servers = configuration_.getServers();
	for (std::vector<ServerDirective>::iterator it = servers.begin(); it != servers.end(); ++it) {
		std::cout << it->getIpAddress() << " " << it->getPort() << std::endl;
		socket_.push_back(server::Socket(it->getIpAddress(), it->getPort()));
	}

	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end();) {
		if (it->initialize() < 0) {
			it = socket_.erase(it);
		} else {
			++it;
		}
	}

	if (socket_.empty()) {
		std::cerr << "Initialization of all addresses failed" << std::endl;
		return -1;
	}
	return 0;
}

int IoMultiplexing::acceptIncomingConnection(int listen_sd) {

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

int IoMultiplexing::disconnect(int sd) {
	close(sd);
	FD_CLR(sd, &master_read_fds_);
	if (sd == highest_socket_descriptor_) {
		while (!FD_ISSET(highest_socket_descriptor_, &master_read_fds_))
			--highest_socket_descriptor_;
	}
	return 0;
}

int IoMultiplexing::request(int sd) {
	bool should_close_connection;
	char buffer[BUFFER_SIZE];

	std::cout << "  Descriptor " << sd << " is readable" << std::endl;
	should_close_connection = false;
	while (1) {
		int result = recv(sd, buffer, sizeof(buffer), 0);
		if (result < 0) {
			std::cerr << "recv() failed: " << strerror(errno) << std::endl;
			should_close_connection = true;
			break;
		}
		http_request_parse_.handleBuffer(sd, buffer);
		if (result == 0) {
			std::cout << "  Connection closed" << std::endl;
			should_close_connection = true;
			break;
		}

		int len = result;
		std::cout << "  " << len << " bytes received\n" << std::endl;

		result = send(sd, buffer, len, 0);
		if (result < 0) {
			std::cerr << "send() failed: " << strerror(errno) << std::endl;
			should_close_connection = true;
			break;
		}
		std::memset(buffer, '\0', sizeof(buffer));
	};

	if (should_close_connection) {
		disconnect(sd);
	}

	return 0;
}

bool IoMultiplexing::isListeningSocket(int sd) {
	for (size_t i = 0; i < socket_.size(); ++i) {
		if (sd == socket_[i].getListenSd()) {
			return true;
		}
	}
	return false;
}

int IoMultiplexing::dispatchSocketEvents(int readyDescriptors) {

	for (int descriptor = 0; descriptor <= highest_socket_descriptor_ && readyDescriptors > 0;
		 ++descriptor) {
		if (FD_ISSET(descriptor, &read_fds__)) {
			if (isListeningSocket(descriptor)) {
				if (acceptIncomingConnection(descriptor) < 0)
					server_is_running_ = false;
			} else {
				request(descriptor);
			}

			--readyDescriptors;
		}
	}
	return 0;
}

int IoMultiplexing::monitorSocketEvents() {
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
		dispatchSocketEvents(select_result);
	}
	return 0;
}

int IoMultiplexing::setupSelectReadFds() {
	FD_ZERO(&master_read_fds_);
	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->getListenSd() > highest_socket_descriptor_) {
			highest_socket_descriptor_ = it->getListenSd();
		}
		FD_SET(it->getListenSd(), &master_read_fds_);
	}
	return 0;
}

int IoMultiplexing::runServer() {
	if (setUpServerSockets() < 0) {
		std::cerr << "setupServerSocket() failed" << std::endl;
		return -1;
	}
	setupSelectReadFds();
	return monitorSocketEvents();
}
}
