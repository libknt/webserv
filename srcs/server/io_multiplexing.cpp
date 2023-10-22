#include "io_multiplexing.hpp"

namespace server {
IoMultiplexing::IoMultiplexing()
	: socket_conf_(std::vector<socket_conf>())
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, max_sd_(-1)
	, max_clients_(-1)
	, end_server_(0) {
	FD_ZERO(&master_set_);
	FD_ZERO(&working_set_);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

IoMultiplexing::IoMultiplexing(std::vector<socket_conf>& conf)
	: socket_conf_(conf)
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, max_sd_(-1)
	, max_clients_(-1)
	, end_server_(0) {
	FD_ZERO(&master_set_);
	FD_ZERO(&working_set_);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

IoMultiplexing::~IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(const IoMultiplexing& other)
	: socket_conf_(other.socket_conf_)
	, socket_(other.socket_)
	, activity_times_(other.activity_times_)
	, max_sd_(other.max_sd_)
	, max_clients_(other.max_clients_)
	, timeout_(other.timeout_)
	, master_set_(other.master_set_)
	, working_set_(other.working_set_)
	, end_server_(other.end_server_) {}

IoMultiplexing& IoMultiplexing::operator=(const IoMultiplexing& other) {

	if (this != &other) {
		socket_conf_ = other.socket_conf_;
		socket_ = other.socket_;
		activity_times_ = other.activity_times_;
		max_sd_ = other.max_sd_;
		max_clients_ = other.max_clients_;
		timeout_ = other.timeout_;
		master_set_ = other.master_set_;
		working_set_ = other.working_set_;
		end_server_ = other.end_server_;
	}
	return *this;
}

int IoMultiplexing::initialize() {
	for (std::vector<socket_conf>::iterator conf_it = socket_conf_.begin();
		 conf_it != socket_conf_.end();
		 ++conf_it) {
		socket_.push_back(server::Socket(conf_it->addr, conf_it->port));
	}

	for (std::vector<server::Socket>::iterator socket_it = socket_.begin();
		 socket_it != socket_.end();) {
		if (socket_it->initialize() < 0) {
			socket_it = socket_.erase(socket_it);
		} else {
			++socket_it;
		}
	}

	if( socket_.empty() ) {
		std::cerr << "Initialization of all addresses failed" << std::endl;
		return -1;
	}
	return 0;
}

int IoMultiplexing::accept(int listen_sd) {

	int new_sd;
	std::cout << "  Listening socket is readable" << std::endl;
	do {
		new_sd = ::accept(listen_sd, NULL, NULL);
		if (new_sd < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				return -1;
			}
			break;
		}

		std::cout << "  New incoming connection -  " << new_sd << std::endl;
		FD_SET(new_sd, &this->master_set_);
		if (new_sd > max_sd_)
			max_sd_ = new_sd;

	} while (new_sd != -1);
	return 0;
}

int IoMultiplexing::disconnection(int i) {
	close(i);
	FD_CLR(i, &master_set_);
	if (i == max_sd_) {
		while (FD_ISSET(max_sd_, &master_set_) == 0)
			max_sd_ -= 1;
	}
	return 0;
}

int IoMultiplexing::request(int i) {
	int close_conn;
	char buffer[1024];

	std::cout << "  Descriptor " << i << " is readable" << std::endl;
	close_conn = 0;
	while (1) {
		int result = recv(i, buffer, sizeof(buffer), 0);
		if (result < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "recv() failed: " << strerror(errno) << std::endl;
				close_conn = 1;
			}
			break;
		}

		if (result == 0) {
			std::cout << "  Connection closed" << std::endl;
			close_conn = 1;
			break;
		}

		int len = result;
		std::cout << "  " << len << " bytes received\n" << std::endl;

		result = send(i, buffer, len, 0);
		if (result < 0) {
			std::cerr << "send() failed: " << strerror(errno) << std::endl;
			close_conn = 1;
			break;
		}
		memset(buffer,'\0', sizeof(buffer));
	};

	if (close_conn) {
		disconnection(i);
	}

	return 0;
}

bool IoMultiplexing::isListeningSocket(int sd) {
	return std::find_if(socket_.begin(), socket_.end(), IsListeningSocketPredicate(sd)) !=
		   socket_.end();
}

int IoMultiplexing::select() {
	while (end_server_ == 0) {
		memcpy(&this->working_set_, &this->master_set_, sizeof(this->master_set_));

		std::cout << "Waiting on select()!" << std::endl;
		int result = ::select(max_sd_ + 1, &this->working_set_, NULL, NULL, &timeout_);

		if (result < 0) {
			std::cerr << "select() failed: " << strerror(errno) << std::endl;
			break;
		}

		if (result == 0) {
			std::cout << "select() timed out.  End program." << std::endl;
			continue;
		}

		int desc_ready = result;
		for (int i = 0; i <= max_sd_ && desc_ready > 0; ++i) {
			if (FD_ISSET(i, &this->working_set_)) {
				desc_ready -= 1;

				if (isListeningSocket(i)) {
					if (accept(i) < 0)
						end_server_ = 1;
				} else {
					request(i);
				}
			}
		}
	}
	return 0;
}

int IoMultiplexing::server_start() {
	FD_ZERO(&master_set_);
	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->getListenSd() > max_sd_) {
			max_sd_ = it->getListenSd();
		}
		FD_SET(it->getListenSd(), &master_set_);
	}
	return select();
}

}