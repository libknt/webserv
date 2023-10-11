#include "io_multiplexing.hpp"

namespace server {
IoMultiplexing::IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(std::vector<socket_conf>& conf)
	: socket_conf_(conf)
	, end_server_(FALSE) {
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
	, working_set_(other.working_set_)
	, end_server_(FALSE) {}

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
		// std::cout << it->getServerAddr() << " : " << it->getPort() << " : " << it->getListenSd()
		// << std::endl;
	}

	return 0;
}

int IoMultiplexing::accept(int i) {
	int listen_sd = -1;
	int new_sd;
	for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
		if (it->getListenSd() == i) {
			listen_sd = i;
			break;
		}
	}
	if (listen_sd != -1) {
		do {
			new_sd = ::accept(listen_sd, NULL, NULL);
			if (new_sd < 0) {
				if (errno != EWOULDBLOCK) {
					std::cerr << "accept() failed: " << strerror(errno) << std::endl;
					end_server_ = TRUE;
				}
				return -1;
			}

			std::cout << "New incoming connection -" << new_sd << std::endl;
			FD_SET(new_sd, &master_set_);
			if (new_sd > max_sd_)
				max_sd_ = new_sd;
		} while (new_sd != -1);
	}
	return 0;
}

int IoMultiplexing::disconnection(int i) {
	close(i);
	FD_CLR(i, &master_set_);
	if (i == max_sd_) {
		while (FD_ISSET(max_sd_, &master_set_) == FALSE)
			max_sd_ -= 1;
	}
	return -1;
}

int IoMultiplexing::request(int i) {
	int result;
	int len;
	char buffer[1024];

	while (true) {
		std::cout << "  Descriptor " << i << " is readable" << std::endl;
		result = recv(i, buffer, sizeof(buffer), 0);
		if (result < 0) {
			if (errno != EWOULDBLOCK) {
				std::cerr << "recv() failed: " << strerror(errno) << std::endl;

				disconnection(i);
			}
			break;
		}

		if (result == 0) {
			std::cout << "Connection closed: " << std::endl;
			disconnection(i);
			break;
		}

		std::cout << buffer << std::endl;

		len = result;
		std::cout << " " << len << " bytes received" << std::endl;

		result = send(i, buffer, len, 0);
		if (result < 0) {
			std::cerr << "send() failed(): " << strerror(errno) << std::endl;
			disconnection(i);
			break;
		}
	}
	return 0;
}

int IoMultiplexing::select() {
	int result;
	int desc_ready;
	int new_sd;
	int close_conn;
	char buffer[1024];
	int len;
	while (end_server_ == FALSE) {
		memcpy(&this->working_set_, &this->master_set_, sizeof(this->master_set_));

		std::cout << "Waiting on select()..." << std::endl;
		result = ::select(max_sd_ + 1, &this->working_set_, NULL, NULL, &this->timeout_);

		if (result < 0) {
			perror("  select() failed");
			break;
		}

		if (result == 0) {
			std::cout << "  select() timed out.  End program." << std::endl;
			break;
		}

		desc_ready = result;
		// max_fdになるまで &&  データが読み取り可能なソケットディスクリプタが無くなるまで
		for (int i = 0; i <= max_sd_ && desc_ready > 0; ++i) {
			// データが読み取り可能なソケットディスクリプタかどうか
			if (FD_ISSET(i, &this->working_set_)) {
				desc_ready -= 1;
				int listen_sd = -1;
				for (std::vector<server::Socket>::iterator it = socket_.begin(); it != socket_.end(); ++it) {
					if (it->getListenSd() == i) {
						listen_sd = i;
						break;
					}
				}
				if (listen_sd != -1) {
					std::cout << "  Listening socket is readable" << std::endl;
					do {
						// 接続は確認された時新しい接続は確認された時, acceptする.
						new_sd = ::accept(listen_sd, NULL, NULL);
						if (new_sd < 0) {
							if (errno != EWOULDBLOCK) {
								perror("  accept() failed");
								end_server_ = TRUE;
							}
							break;
						}

						std::cout << "  New incoming connection -  " << new_sd << std::endl;
						// 新しい接続をmasterにセットするx
						FD_SET(new_sd, &this->master_set_);
						if (new_sd > max_sd_)
							max_sd_ = new_sd;

					} while (new_sd != -1);
				}
				// 既存のclient接続に関する
				else {
					std::cout << "  Descriptor " << i << " is readable" << std::endl;
					close_conn = FALSE;
					while (TRUE) {
						// データの受信 return_value[size]
						result = recv(i, buffer, sizeof(buffer), 0);
						if (result < 0) {
							if (errno != EWOULDBLOCK) {
								perror("  recv() failed");
								close_conn = TRUE;
							}
							break;
						}

						if (result == 0) {
							std::cout << "  Connection closed" << std::endl;
							close_conn = TRUE;
							break;
						}

						len = result;
						std::cout << "  " << len << " bytes received\n" << std::endl;

						result = send(i, buffer, len, 0);
						if (result < 0) {
							perror("  send() failed");
							close_conn = TRUE;
							break;
						}
					};

					if (close_conn) {
						close(i);
						FD_CLR(i, &this->master_set_);
						if (i == max_sd_) {
							while (FD_ISSET(max_sd_, &this->master_set_) == FALSE)
								max_sd_ -= 1;
						}
					}
				}
			}
		}
	}
	// while (end_server_ == FALSE) {
	// 	memcpy(&working_set_, &master_set_, sizeof(master_set_));

	// 	std::cout << "Waitng on select()" << std::endl;
	// 	result = ::select(max_sd_ + 1, &working_set_, NULL, NULL, &timeout_);
	// 	// result = ::select(max_sd_ + 1, &working_set_, NULL, NULL, NULL);
	// 	if (result < 0) {
	// 		std::cerr << "select() failed: " << strerror(errno) << std::endl;
	// 		return -1;
	// 	}

	// 	if (result == 0) {
	// 		std::cout << "  select() timed out.  End program." << std::endl;
	// 		break;
	// 	}

	// 	desc_ready = result;
	// 	for (int i = 0; i <= max_sd_ && desc_ready > 0; ++i) {
	// 		if (FD_ISSET(i, &working_set_)) {
	// 			--desc_ready;
	// 			if (accept(i) < 0)
	// 				break;
	// 		} else {
	// 			if (request(i) < 0)
	// 				break;
	// 		}
	// 	}
	// }
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