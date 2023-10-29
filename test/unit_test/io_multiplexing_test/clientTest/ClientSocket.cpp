#include "ClientSocket.hpp"

http::ClientSocket::ClientSocket(const char* server_address, int port)
	: server_address_(server_address)
	, port_(port)
	, client_socket_(-1) {
	std::memset(&addr_, 0, sizeof(addr_));
}

http::ClientSocket::~ClientSocket() {
	std::cout << "ClientSocket default destractor call client_socket_: " << client_socket_
			  << std::endl;
	if (client_socket_ >= 0) {
		close(client_socket_);
		client_socket_ = -1;
	}
}

int http::ClientSocket::socket() {
	client_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket_ < 0) {
		std::cerr << "socker() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ClientSocket::set() {

	addr_.sin_family = AF_INET;
	if (inet_pton(AF_INET, server_address_, &(addr_.sin_addr)) <= 0) {
		perror("inet_pton() failed");
		std::cerr << "inet_pton(): " << strerror(errno) << std::endl;
		return -1;
	}
	addr_.sin_port = htons(port_);
	return 0;
}

int http::ClientSocket::connect() {
	int rc = ::connect(client_socket_, (struct sockaddr*)&addr_, sizeof(addr_));
	if (rc < 0) {
		std::cerr << "connect() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

int http::ClientSocket::initialize() {
	if (socket() < 0)
		return -1;
	if (set() < 0)
		return -1;
	if (connect() < 0)
		return -1;
	return 0;
}

const char* http::ClientSocket::getServerAddr_() const {
	return server_address_;
}
int http::ClientSocket::getPort_() const {
	return port_;
}
int http::ClientSocket::getClientSocket() const {
	return client_socket_;
}