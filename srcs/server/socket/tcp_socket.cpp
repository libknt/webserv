#include "tcp_socket.hpp"

namespace server {

TcpSocket::TcpSocket(std::string server_address, std::string port)
	: ip_address_(server_address)
	, port_(port)
	, listen_sd_(-1)
	, backlog_(SOMAXCONN) {
	std::memset(&socket_address_, 0, sizeof(socket_address_));
}

TcpSocket::TcpSocket(const TcpSocket& other)
	: ip_address_(other.ip_address_)
	, port_(other.port_)
	, listen_sd_(other.listen_sd_)
	, backlog_(other.backlog_) {
	std::memcpy(&socket_address_, &other.socket_address_, sizeof(other.socket_address_));
}

TcpSocket& TcpSocket::operator=(const TcpSocket& other) {
	if (this != &other) {
		ip_address_ = other.ip_address_;
		port_ = other.port_;
		listen_sd_ = other.listen_sd_;
		backlog_ = other.backlog_;
		std::memcpy(&socket_address_, &other.socket_address_, sizeof(other.socket_address_));
	}
	return *this;
}

TcpSocket::~TcpSocket() {
	if (listen_sd_ >= 0) {
		close(listen_sd_);
		listen_sd_ = -1;
	}
}

bool TcpSocket::isSocketConfigValid() {
	struct sockaddr_in socket_address;
	if (inet_pton(AF_INET, ip_address_.c_str(), &(socket_address.sin_addr)) <= 0) {
		return false;
	}

	return backlog_ > 0 && backlog_ <= SOMAXCONN;
}

int TcpSocket::createTcpSocket() {
	const struct protoent* protocol_info = getprotobyname("tcp");
	if (!protocol_info) {
		std::cerr << "getprotobyname() failed: " << strerror(errno) << std::endl;
		return -1;
	}

	listen_sd_ = socket(AF_INET, SOCK_STREAM, protocol_info->p_proto);
	if (listen_sd_ < 0) {
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int TcpSocket::enableSocketAddressReuse() {
	int enable_address_reuse = 1;
	if (setsockopt(listen_sd_,
			SOL_SOCKET,
			SO_REUSEADDR,
			reinterpret_cast<char*>(&enable_address_reuse),
			sizeof(enable_address_reuse)) < 0) {
		std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int TcpSocket::setSocketToNonBlocking() {
	int flags = fcntl(listen_sd_, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "fcntl() get flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	flags |= O_NONBLOCK;
	flags = fcntl(listen_sd_, F_SETFL, flags);
	if (flags < 0) {
		std::cerr << "fcntl() set flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 0;
}

int TcpSocket::configureSocketAddress() {
	struct addrinfo address_info_hints, *address_info_results, *address_iterator;
	int address_info_status;

	std::memset(&address_info_hints, 0, sizeof(address_info_hints));
	address_info_hints.ai_family = AF_INET;
	address_info_hints.ai_socktype = SOCK_STREAM;
	address_info_hints.ai_flags = AI_PASSIVE;

	if ((address_info_status = getaddrinfo(
			 ip_address_.c_str(), port_.c_str(), &address_info_hints, &address_info_results)) !=
		0) {
		std::cerr << "getaddrinfo() failed: " << gai_strerror(address_info_status) << std::endl;
		return -1;
	}

	for (address_iterator = address_info_results; address_iterator != NULL;
		 address_iterator = address_iterator->ai_next) {
		if (address_iterator->ai_family == AF_INET) {
			std::memcpy(&socket_address_, address_iterator->ai_addr, address_iterator->ai_addrlen);
			break;
		}
	}
	freeaddrinfo(address_info_results);
	if (address_iterator == NULL) {
		std::cerr << "Failed to set socket address." << std::endl;
		return -1;
	}

	return 0;
}

int TcpSocket::bindAddressToSocket() {
	if (bind(listen_sd_,
			reinterpret_cast<struct sockaddr*>(&socket_address_),
			sizeof(socket_address_)) < 0) {
		std::cerr << "bind() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int TcpSocket::prepareSocketForListening() {
	if (!isSocketConfigValid())
		return -1;
	if (createTcpSocket() < 0)
		return -1;
	if (enableSocketAddressReuse() < 0)
		return -1;
	if (setSocketToNonBlocking() < 0)
		return -1;
	if (configureSocketAddress() < 0)
		return -1;
	if (bindAddressToSocket() < 0)
		return -1;
	return 0;
}

int TcpSocket::startListening() {
	if (listen(listen_sd_, backlog_) < 0) {
		std::cerr << "listen() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	std::cout << "Socket " << listen_sd_ << " started listening on " << ip_address_ << ":" << port_
			  << std::endl;
	return 0;
}

std::string const& TcpSocket::getIpAddress() const {
	return ip_address_;
}

std::string const& TcpSocket::getPort() const {
	return port_;
}

int TcpSocket::getListenSd() const {
	return listen_sd_;
}

int TcpSocket::getBacklog() const {
	return backlog_;
}

struct sockaddr_in const& TcpSocket::getSocketAddress() const {
	return socket_address_;
}

std::ostream& operator<<(std::ostream& out, const TcpSocket& tcp_socket) {
	out << "TcpSocket: " << std::endl;
	out << "  ip_address: " << tcp_socket.getIpAddress() << std::endl;
	out << "  port: " << tcp_socket.getPort() << std::endl;
	out << "  listen_sd: " << tcp_socket.getListenSd() << std::endl;
	out << "  backlog: " << tcp_socket.getBacklog() << std::endl;
	out << "  socket_address: " << std::endl;
	out << "    sin_family: " << tcp_socket.getSocketAddress().sin_family << std::endl;
	out << "    sin_port: " << tcp_socket.getSocketAddress().sin_port << std::endl;
	out << "    sin_addr: " << tcp_socket.getSocketAddress().sin_addr.s_addr << std::endl;
	return out;
}

}