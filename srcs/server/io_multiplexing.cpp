#include "io_multiplexing.hpp"

namespace server {
IoMultiplexing::IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(std::vector<socket_conf>& conf)
	: socket_conf_(conf) {
	timeout_.tv_sec = 3 * 60;
	timeout_.tv_usec = 0;
	}

IoMultiplexing::~IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(const IoMultiplexing& other)
	: socket_(other.socket_)
	, max_sd_(other.max_sd_)
	, max_clients_(other.max_clients_)
	, timeout_(other.timeout_)
	, master_set_(other.master_set_)
	, working_set_(other.working_set_) {}

IoMultiplexing& IoMultiplexing::operator=(const IoMultiplexing& other) {

	if (this != &other) {
		socket_ = other.socket_;
		max_sd_ = other.max_sd_;
		max_clients_ = other.max_clients_;
		timeout_ = other.timeout_;
		master_set_ = other.master_set_;
		working_set_ = other.working_set_;
	}
	return *this;
}

int IoMultiplexing::initialize() {
	for (std::vector<socket_conf>::iterator it = socket_conf_.begin(); it != socket_conf_.end();
		 ++it) {
		socket_conf conf = *it;
		const char* addr = conf.addr;
		int port = conf.port;
		server::Socket socket(addr, port);
		socket_.push_back(socket);
	}

	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->initialize() < 0)
			return -1;
		// std::cout << it->getServerAddr() << " : " << it->getPort() << " : " << it->getListenSd() << std::endl;
	}

	return 0;
}

int IoMultiplexing::server_start() {
	FD_ZERO(&master_set_);
	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->getListenSd() > max_sd_) {
			max_sd_ = it->getListenSd();
		}
		FD_SET(it->getListenSd(),&master_set_);
	}

	
	return 0;
}

}