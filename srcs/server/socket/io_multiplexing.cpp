#include "io_multiplexing.hpp"
#include "parse_http_request.hpp"

namespace server {
IoMultiplexing::IoMultiplexing()
	: request_process_status_(std::map<int, RequestProcessStatus>())
	, socket_conf_(std::vector<socket_conf>())
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, max_sd_(-1)
	, max_clients_(-1)
	, should_stop_server_(false)
	, response_(std::map<int, std::string>()) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&read_fds_);
	FD_ZERO(&master_write_fds_);
	FD_ZERO(&write_fds_);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

IoMultiplexing::IoMultiplexing(std::vector<socket_conf>& conf)
	: request_process_status_(std::map<int, RequestProcessStatus>())
	, socket_conf_(conf)
	, socket_(std::vector<server::Socket>())
	, activity_times_(std::map<int, time_t>())
	, max_sd_(-1)
	, max_clients_(-1)
	, should_stop_server_(false) {
	FD_ZERO(&master_read_fds_);
	FD_ZERO(&read_fds_);
	FD_ZERO(&master_write_fds_);
	FD_ZERO(&write_fds_);
	timeout_.tv_sec = 10;
	timeout_.tv_usec = 0;
}

IoMultiplexing::~IoMultiplexing() {}

IoMultiplexing::IoMultiplexing(const IoMultiplexing& other)
	: request_process_status_(other.request_process_status_)
	, socket_conf_(other.socket_conf_)
	, socket_(other.socket_)
	, activity_times_(other.activity_times_)
	, max_sd_(other.max_sd_)
	, max_clients_(other.max_clients_)
	, timeout_(other.timeout_)
	, master_read_fds_(other.master_read_fds_)
	, read_fds_(other.read_fds_)
	, master_write_fds_(other.master_write_fds_)
	, write_fds_(other.write_fds_)
	, should_stop_server_(other.should_stop_server_) {}

IoMultiplexing& IoMultiplexing::operator=(const IoMultiplexing& other) {

	if (this != &other) {
		request_process_status_ = other.request_process_status_;
		socket_conf_ = other.socket_conf_;
		socket_ = other.socket_;
		activity_times_ = other.activity_times_;
		max_sd_ = other.max_sd_;
		max_clients_ = other.max_clients_;
		timeout_ = other.timeout_;
		master_read_fds_ = other.master_read_fds_;
		read_fds_ = other.read_fds_;
		master_write_fds_ = other.master_write_fds_;
		write_fds_ = other.write_fds_;
		should_stop_server_ = other.should_stop_server_;
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
	FD_CLR(sd, &master_write_fds_);
	if (sd == max_sd_) {
		while (!FD_ISSET(max_sd_, &master_read_fds_))
			--max_sd_;
	}
	request_process_status_.erase(sd);
	http_request_parse_.httpRequestErase(sd);
	return 0;
}

int IoMultiplexing::recv(int sd) {
	char buffer[BUFFER_SIZE];

	std::cout << "  Descriptor " << sd << " is readable" << std::endl;
	int result = ::recv(sd, buffer, sizeof(buffer), 0);
	if (result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		disconnect(sd);
		return -1;
	}
	if (result == 0) {
		std::cout << "  Connection closed" << std::endl;
		disconnect(sd);
		return -1;
	}
	request_process_status_[sd] = http_request_parse_.handleBuffer(sd, buffer);
	if (request_process_status_[sd] == ERROR) {
	}

	if (request_process_status_[sd] == REQUESTFINISH) {
		std::cout << "request finish !!!" << std::endl;
	}

	return 0;
}

int IoMultiplexing::send(int sd) {

	char buffer[BUFFER_SIZE];

	std::memset(buffer, 0, BUFFER_SIZE);

	size_t size;
	if (response_[sd].size() < BUFFER_SIZE) {
		size = response_[sd].size();
	} else {
		size = BUFFER_SIZE;
	}
	std::memcpy(buffer, response_[sd].c_str(), size);
	response_[sd] = response_[sd].substr(size);
	std::cout << buffer << std::endl;

	int result = ::send(sd, buffer, size, 0);
	if (result < 0) {
		std::cerr << "send() failed: " << strerror(errno) << std::endl;
		disconnect(sd);
		return -1;
	}
	if (result == 0) {
		std::cout << "  Connection closed" << std::endl;
		disconnect(sd);
		return -1;
	}

	if (response_.empty()) {
		request_process_status_[sd] = FINISH;
	}

	if (request_process_status_[sd] == FINISH) {
		disconnect(sd);
	}

	std::cout << sd << "  " << size << " bytes sended\n" << std::endl;
	return 0;
}

bool IoMultiplexing::isListeningSocket(int sd) {
	return std::find_if(socket_.begin(), socket_.end(), IsListeningSocketPredicate(sd)) !=
		   socket_.end();
}

bool IoMultiplexing::isCgi() {
	return true;
}

// cgiかhttp responseかどうか調べてsetする.
void IoMultiplexing::setResponseStatus(int sd) {
	// if (isCgi) {
	// request_process_status_[sd] = CGI;
	// } else {
	// request_process_status_[sd] = RESPONSE;
	// }
	request_process_status_[sd] = SEND;
	response_[sd] = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 1098\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n\r\n    <title>Extended Simple Response</title>\r\n</head>\r\n<body>\r\n\r\n    <h1>Hello, this is an extended simple HTTP response!</h1>\r\n\r\n<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus lacinia odio vitae vestibulum. Donec in efficitur leo. In nisl ligula, vulputate id diam in, condimentum tincidunt sapien. Maecenas gravida velit vitae dolor finibus, in feugiat urna interdum. Vestibulum euismod euismod velit, nec feugiat lacus feugiat sit amet. Vestibulum consectetur sit amet lacus a pellentesque. Duis tincidunt, est sed sodales tincidunt, ipsum erat elementum massa, at lacinia arcu ex a est. Fusce ut congue metus, sed tincidunt urna.</p>\r\n\r\n<p>Quisque egestas eget lacus non condimentum. Sed at imperdiet dui, vel facilisis velit. Proin ac neque nec arcu commodo aliquet. Morbi tincidunt turpis et tincidunt. Fusce id dui id libero aliquet sagittis a at libero. Nulla at libero pharetra, bibendum metus sed, malesuada metus. Cras sollicitudin, quam pellentesque lobortis auctor, ante eros bibendum nunc, sed bibendum quam ex non metus. Curabitur in pharetra odio, in efficitur leo.</p>\r\n</body>\r\n</html>";
}

int IoMultiplexing::select() {
	while (should_stop_server_ == false) {
#ifdef FD_COPY
		FD_COPY(&master_read_fds_, &read_fds_);
		FD_COPY(&master_write_fds_, &write_fds_);
#else
		std::memcpy(&read_fds_, &master_read_fds_, sizeof(master_read_fds_));
		std::memcpy(&write_fds_, &master_write_fds_, sizeof(master_write_fds_));
#endif

		std::cout << "Waiting on select()!" << std::endl;
		int result = ::select(max_sd_ + 1, &read_fds_, &write_fds_, NULL, &timeout_);

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
			if (FD_ISSET(sd, &read_fds_)) {
				if (isListeningSocket(sd)) {
					if (accept(sd) < 0)
						should_stop_server_ = true;
				} else {
					if (recv(sd) < 0) {
						--desc_ready;
						continue;
					}
					if (request_process_status_[sd] == REQUESTFINISH) {
						setResponseStatus(sd);
						// response execute
						if (request_process_status_[sd] == RESPONSE) {
							// レスポンスの作成
							// そして、ステータスの変更
						}
						if (request_process_status_[sd] == SEND) {
							FD_SET(sd, &master_write_fds_);
							--desc_ready;
							continue;
						}
						// cgi execute
						if (request_process_status_[sd] == CGI) {
							// 何かしらのcgi実行
							--desc_ready;
							continue;
						}
					}
				}
				--desc_ready;
			} else if (FD_ISSET(sd, &write_fds_)) {
				// if(cgi)
				if (request_process_status_[sd] == CGI_SEND_BODY) {
					// 子プロセスにbodyを送る
				}
				// TODO
				//  if (request_process_status_[sd] == CGI_LOCAL_REDIRECT) {}
				if (request_process_status_[sd] == SEND) {
					send(sd);
				}
				--desc_ready;
			}
		}
	}
	return 0;
}

int IoMultiplexing::server_start() {
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
