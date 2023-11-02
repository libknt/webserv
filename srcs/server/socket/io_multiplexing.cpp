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
	, response_(std::map<int, HttpResponse>()) {
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
	timeout_.tv_sec = 6;
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
		std::cout << " request handle err" << std::endl;
		disconnect(sd);
		return -1;
	}

	return 0;
}

int IoMultiplexing::send(int sd) {

	char buffer[BUFFER_SIZE];
	size_t	buffer_size = 0;

	std::map<int, HttpResponse>::iterator it = response_.find(sd);

	// ここでresponseで返すファイルをreadしながら、stream_に投げて、stream_からBUFFER_SIZEだけbufferに書き込む。
	if (!ofs_[sd].eof())
	{
		memset(buffer, '\0', BUFFER_SIZE);
		ofs_[sd].read(buffer, BUFFER_SIZE - 1);
		response_[sd].addStream(std::string(buffer));
	}
	//request_process_status_[sd] = it->second.setSendBuffer(buffer, BUFFER_SIZE);
	request_process_status_[sd] = it->second.setSendBuffer2(buffer, buffer_size, BUFFER_SIZE);
	//std::cout << buffer << std::endl;
	int result = ::send(sd, buffer, buffer_size, 0);
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

	if (request_process_status_[sd] == FINISH) {
		std::cout << "debug: hello" << std::endl;
		disconnect(sd);
		http_request_parse_.httpRequestErase(sd);
		response_.erase(sd);
		ofs_[sd].close();
	}

	std::cout << sd << "  " << sizeof(buffer) << " bytes sended\n" << std::endl;
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
	if (request_process_status_[sd] == REQUESTFINISH)
		request_process_status_[sd] = RESPONSE;
	// }
	return;
}

int IoMultiplexing::createResponse(int sd) {
	if (request_process_status_[sd] == RESPONSE) {
		// ここではrequestを用いて適切なreponseの構築と対象ファイルのオープンを行う。
		HttpRequest const request = http_request_parse_.getHttpRequest(sd);
		response_[sd] = executeRequest(request);
		//TODO ファイルが開かなかった時のエラー処理が必要。多分開くけど。
		ofs_[sd].open(response_[sd].getFilePath().c_str());
		if (!ofs_[sd])
			std::cout << "out" << std::endl;
	} else if (request_process_status_[sd] == CGI) {
	}
	request_process_status_[sd] = SEND;
	FD_SET(sd, &master_write_fds_);
	return 0;
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
					if (request_process_status_[sd] == RECV && recv(sd) < 0) {
						--desc_ready;
						continue;
					}
					if (request_process_status_[sd] == REQUESTFINISH) {
						setResponseStatus(sd);
						//TODO sdがmax_sdなら、二番目に小さいものをmax_sdにせっていするべき。
						FD_CLR(sd, &read_fds_);
						if (createResponse(sd) < 0) {
							--desc_ready;
							continue;
						}
					}
					--desc_ready;
				}
			} else if (FD_ISSET(sd, &write_fds_)) {
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
