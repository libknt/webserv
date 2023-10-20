#include "socket.hpp"

namespace server {
Socket::Socket(const char* server_addr, int port)
	: server_addr_(server_addr)
	, port_(port)
	, listen_sd_(-1)
	, backlog_(SOMAXCONN) {
	memset(&addr_, 0, sizeof(addr_));
}

Socket::Socket(const char* server_addr, int port, int backlog)
	: server_addr_(server_addr)
	, port_(port)
	, listen_sd_(-1)
	, backlog_(backlog) {
	memset(&addr_, 0, sizeof(addr_));
}

Socket::Socket(const Socket& other)
	: server_addr_(other.server_addr_)
	, port_(other.port_)
	, listen_sd_(other.listen_sd_)
	, backlog_(other.backlog_) {
	memcpy(&addr_, &other.addr_, sizeof(other.addr_));
}

Socket& Socket::operator=(const Socket& other) {
	if (this != &other) {
		server_addr_ = other.server_addr_;
		port_ = other.port_;
		listen_sd_ = other.listen_sd_;
		backlog_ = other.backlog_;
		memcpy(&addr_, &other.addr_, sizeof(other.addr_));
	}
	return *this;
}

Socket::~Socket() {
	if (listen_sd_ >= 0) {
		close(listen_sd_);
		listen_sd_ = -1;
	}
}

int Socket::socket() {
    struct protoent *protoinfo = getprotobyname("tcp");
    if (!protoinfo) {
        std::cerr << "getprotobyname() failed: " << strerror(errno) << std::endl;
        return -1;
    }

    int protocol = protoinfo->p_proto;

    this->listen_sd_ = ::socket(AF_INET, SOCK_STREAM, protocol);
    if (this->listen_sd_ < 0) {
        std::cerr << "socket() failed: " << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}


int Socket::setsockopt() {
	int is_on = 1;
	int rc = ::setsockopt(
		this->listen_sd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&is_on), sizeof(is_on));
	if (rc < 0) {
		std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int Socket::nonBlock() {
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

int Socket::setSocketAddress() {
	struct addrinfo hints, *res, *p;
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::ostringstream oss;
	oss << this->port_;
	std::string port_str = oss.str();

	if ((status = getaddrinfo(this->server_addr_, port_str.c_str(), &hints, &res)) != 0) {
		std::cerr << "getaddrinfo() failed: " << gai_strerror(status) << std::endl;
		return -1;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		if (p->ai_family == AF_INET || p->ai_family == AF_INET6) {
			memcpy(&addr_, p->ai_addr, p->ai_addrlen);
			break;
		}
	}

	freeaddrinfo(res);

	if (p == NULL) {
		std::cerr << "Failed to set socket address." << std::endl;
		return -1;
	}

	return 0;
}

// int Socket::setSocketAddress() {
// 	this->addr_.sin_family = AF_INET;
// 	if (inet_pton(AF_INET, this->server_addr_, &(addr_.sin_addr)) <= 0) {
// 		std::cerr << "inet_pton() failed" << strerror(errno) << std::endl;
// 		return -1;
// 	}
// 	addr_.sin_port = htons(this->port_);
// 	return 0;
// }

int Socket::bind() {
	int rc = ::bind(this->listen_sd_, reinterpret_cast<struct sockaddr*>(&addr_), sizeof(addr_));
	if (rc < 0) {
		std::cerr << "bind() failed" << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int Socket::listen() {
	int rc = ::listen(this->listen_sd_, this->backlog_);
	if (rc < 0) {
		std::cerr << "listen() failed" << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

bool Socket::isValid() {
	if (port_ < min_port_ || port_ > max_port_)
		return false;

	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, server_addr_, &(sa.sin_addr));
	if (result <= 0)
		return false;

	return backlog_ > 0 && backlog_ <= SOMAXCONN;
}

int Socket::initialize() {
	if (!this->isValid())
		return -1;
	if (this->socket() < 0)
		return -1;
	if (this->setsockopt() < 0)
		return -1;
	if (this->nonBlock() < 0)
		return -1;
	if (this->setSocketAddress() < 0)
		return -1;
	if (this->bind() < 0)
		return -1;
	if (this->listen() < 0)
		return -1;
	std::cout << "Socket initialization succeeded!! address: " << this->server_addr_
			  << " port: " << this->port_ << std::endl;
	return 0;
}

const char* Socket::getServerAddr() const {
	return this->server_addr_;
}

int Socket::getPort() const {
	return this->port_;
}

int Socket::getListenSd() const {
	return this->listen_sd_;
}

}