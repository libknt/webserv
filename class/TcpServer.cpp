#include "TcpServer.hpp"

http::TcpServer::TcpServer(const char* server_addr, int port, int max_clients)
	: server_socket_(server_addr, port)
	, max_sd_(-1)
	, max_clients_(max_clients) {

	FD_ZERO(&master_set_);
	FD_ZERO(&working_set_);
	timeout_.tv_sec = 3 * 60;
	timeout_.tv_usec = 0;
}

http::TcpServer::~TcpServer() {
	std::cout << "TcpServer default destractor call." << std::endl;
	for (int i = 0; i <= max_sd_; ++i) {
		if (FD_ISSET(i, &master_set_))
			close(i);
	}
}

int http::TcpServer::initialize() {
	if (this->server_socket_.initialize())
		return -1;
	this->max_sd_ = this->server_socket_.getListenSd();
	FD_SET(this->server_socket_.getListenSd(), &this->master_set_);
	return 0;
}

int http::TcpServer::start() {
	int end_server = FALSE;

	while (end_server == FALSE) {
		memcpy(&this->working_set_, &this->master_set_, sizeof(this->master_set_));

		std::cout << "Waiting on select()..." << std::endl;
		int rc = ::select(this->max_sd_ + 1, &this->working_set_, NULL, NULL, &this->timeout_);
		if (rc < 0) {
			std::cerr << "select() failed: " << strerror(errno) << std::endl;
			return -1;
		}
		if (rc == 0) {
			std::cout << "select() timeout. End Proguram." << std::endl;
			return 1;
		}

		int desc_ready = rc;

		for (int i = 0; i <= max_sd_ && desc_ready > 0; ++i) {
			if (FD_ISSET(i, &working_set_)) {
				--desc_ready;
				if (i == this->server_socket_.getListenSd()) {
					std::cout << "  Listening socket is readable" << std::endl;
					int new_sd = 0;
					while (new_sd != -1) {
						new_sd = accept(this->server_socket_.getListenSd(), NULL, NULL);
						if (new_sd < 0) {
							if (errno != EWOULDBLOCK) {
								std::cerr << "accept() failed: " << strerror(errno) << std::endl;
								end_server = TRUE;
							}
							break;
						}

						std::cout << "  New incoming connection -  " << new_sd << std::endl;
						FD_SET(new_sd, &master_set_);
						if (new_sd > max_sd_)
							max_sd_ = new_sd;
					}

				} else {
					std::cout << "  Descriptor " << i << " is readable" << std::endl;
					int close_conn = FALSE;
					while (TRUE) {
						rc = ::recv(i, buffer_, sizeof(buffer_), 0);
						if (rc < 0) {
							if (errno != EWOULDBLOCK) {
								std::cerr << "recv() failed: " << strerror(errno) << std::endl;
								close_conn = TRUE;
							}
							break;
						}

						if (rc == 0) {
							std::cout << "Connection closed." << std::endl;
							close_conn = TRUE;
							break;
						}

						int len = rc;
						std::cout << "  " << len << " bytes received" << std::endl;

						rc = ::send(i, buffer_, len, 0);
						if (rc < 0) {
							std::cerr << "send() failed: " << strerror(errno) << std::endl;
							close_conn = TRUE;
							break;
						}
						// sleep(1);
					};

					if (close_conn) {
						close(i);
						FD_CLR(i, &master_set_);
						if (i == max_sd_) {
							while (FD_ISSET(max_sd_, &master_set_) == FALSE)
								max_sd_ -= 1;
						}
					}
				}
			}
		}
	}
	return 0;
}