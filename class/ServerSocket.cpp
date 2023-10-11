#include "ServerSocket.hpp"

http::ServerSocket::ServerSocket(const char* server_addr, int port)
	: server_addr_(server_addr)
	, port_(port)
	, listen_sd_(-1)
	, backlog_(30) {
	memset(&addr, 0, sizeof(addr));
}

http::ServerSocket::~ServerSocket() {
	std::cout << "ServerSocket default destractor call listen_sd_: " << listen_sd_ << std::endl;
	if (listen_sd_ >= 0) {
		close(listen_sd_);
		listen_sd_ = -1;
	}
}

int http::ServerSocket::socket() {
	this->listen_sd_ = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->listen_sd_ < 0) {
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ServerSocket::setsockopt() {
	int on = 1;
	int rc = ::setsockopt(this->listen_sd_, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	if (rc < 0) {
		std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ServerSocket::nonBlock() {
	int rc = ::fcntl(this->listen_sd_, F_GETFL, 0);
	if (rc < 0) {
		std::cerr << "fcntl() get flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	rc |= O_NONBLOCK;
	rc = fcntl(this->listen_sd_, F_SETFL, rc);
	if (rc < 0) {
		std::cerr << "fcntl() set flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 0;
}

int http::ServerSocket::setSockaddr() {
	this->addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, this->server_addr_, &(addr.sin_addr)) <= 0) {
		std::cerr << "inet_pton() failed" << strerror(errno) << std::endl;
		return -1;
	}
	addr.sin_port = htons(this->port_);
	return 0;
}

int http::ServerSocket::bind() {
	int rc = ::bind(this->listen_sd_, (struct sockaddr*)&addr, sizeof(addr));
	if (rc < 0) {
		std::cerr << "bind() failed" << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ServerSocket::listen() {
	int rc = ::listen(this->listen_sd_, this->backlog_);
	if (rc < 0) {
		std::cerr << "listen() failed" << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ServerSocket::initialize() {
	if (this->socket() < 0)
		return -1;
	if (this->setsockopt() < 0)
		return -1;
	if (this->nonBlock() < 0)
		return -1;
	if (this->setSockaddr() < 0)
		return -1;
	if (this->bind() < 0)
		return -1;
	if (this->listen() < 0)
		return -1;

	return 0;
}

const char* http::ServerSocket::getServerAddr_() const {
	return this->server_addr_;
}

int http::ServerSocket::getPort_() const {
	return this->port_;
}

int http::ServerSocket::getListenSd() const {
	return this->listen_sd_;
}
