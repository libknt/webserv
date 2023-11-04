#include "io_multiplexing.hpp"
#include "parse_http_request.hpp"

namespace server {

IoMultiplexing::IoMultiplexing(Configuration& configuration)
	: configuration_(configuration)
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, max_sd_(-1)
	, max_clients_(-1)
	, should_stop_server_(false) {
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
	, max_sd_(other.max_sd_)
	, max_clients_(other.max_clients_)
	, timeout_(other.timeout_)
	, master_read_fds_(other.master_read_fds_)
	, read_fds__(other.read_fds__)
	, should_stop_server_(other.should_stop_server_) {}

IoMultiplexing& IoMultiplexing::operator=(const IoMultiplexing& other) {
	if (this != &other) {
		configuration_ = other.configuration_;
		socket_ = other.socket_;
		activity_times_ = other.activity_times_;
		max_sd_ = other.max_sd_;
		max_clients_ = other.max_clients_;
		timeout_ = other.timeout_;
		master_read_fds_ = other.master_read_fds_;
		read_fds__ = other.read_fds__;
		should_stop_server_ = other.should_stop_server_;
	}
	return *this;
}

int IoMultiplexing::initialize() {
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

int IoMultiplexing::accept(int listen_sd) {

	int new_sd;
	std::cout << "  Listening socket is readable" << std::endl;
	do {
		sockaddr_in client_address;
		socklen_t addr_len = sizeof(client_address);
		memset(&client_address, 0, sizeof(client_address));
		new_sd = ::accept(listen_sd, (sockaddr*)&client_address, &addr_len);
		if (new_sd < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				return -1;
			}
			break;
		}
		sockaddr_in server_address;
		addr_len = sizeof(server_address);

		if (getsockname(new_sd, (struct sockaddr*)&server_address, &addr_len) == -1) {
			std::cerr << strerror(errno) << std::endl;
			return -1;
		}

		http_request_parse_.addAcceptClientInfo(new_sd, client_address, server_address);

		std::cout << "  New incoming connection -  " << new_sd << std::endl;
		FD_SET(new_sd, &master_read_fds_);
		if (new_sd > max_sd_)
			max_sd_ = new_sd;

	} while (new_sd != -1);
	return 0;
}

int IoMultiplexing::disconnect(int sd) {
	close(sd);
	FD_CLR(sd, &master_read_fds_);
	if (sd == max_sd_) {
		while (!FD_ISSET(max_sd_, &master_read_fds_))
			--max_sd_;
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
	return std::find_if(socket_.begin(), socket_.end(), IsListeningSocketPredicate(sd)) !=
		   socket_.end();
}

int IoMultiplexing::select() {
	while (should_stop_server_ == false) {
		std::memcpy(&read_fds__, &master_read_fds_, sizeof(master_read_fds_));

		std::cout << "Waiting on select()!" << std::endl;
		int result = ::select(max_sd_ + 1, &read_fds__, NULL, NULL, &timeout_);

		if (result < 0) {
			std::cerr << "select() failed: " << strerror(errno) << std::endl;
			break;
		}

		if (result == 0) {
			std::cout << "select() timed out.  End program." << std::endl;
			continue;
		}

		int desc_ready = result;

		for (int sd = 0; sd <= max_sd_ && desc_ready > 0; ++sd) {
			if (FD_ISSET(sd, &read_fds__)) {
				if (isListeningSocket(sd)) {
					if (accept(sd) < 0)
						should_stop_server_ = true;
				} else {
					request(sd);
				}

				--desc_ready;
			}
		}
	}
	return 0;
}

int IoMultiplexing::run_server() {
	FD_ZERO(&master_read_fds_);
	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->getListenSd() > max_sd_) {
			max_sd_ = it->getListenSd();
		}
		FD_SET(it->getListenSd(), &master_read_fds_);
	}
	return select();
}
}
