#include "cgi.hpp"

namespace server {

Cgi::Cgi(HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: cgi_request_context_(CgiRequestContext(request, client_address, server_address))
	, pid_(-1)
	, status_(-1) {
	socket_vector_[0] = -1;
	socket_vector_[1] = -1;
}

Cgi::Cgi(const Cgi& other)
	: cgi_request_context_(other.cgi_request_context_)
	, pid_(other.pid_)
	, status_(other.status_) {
	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		cgi_request_context_ = other.cgi_request_context_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		pid_ = other.pid_;
		status_ = other.status_;
	}
	return *this;
}

Cgi::~Cgi() {}

int Cgi::setNonBlocking(int sd) {
	int flags = fcntl(sd, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "fcntl() get flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	flags |= O_NONBLOCK;
	flags = fcntl(sd, F_SETFL, flags);
	if (flags < 0) {
		std::cerr << "fcntl() set flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 0;
}

int Cgi::setupInterProcessCommunication() {
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vector_) == -1) {
		std::cerr << "socketpair() failed(): " << strerror(errno) << std::endl;
		return -1;
	}
	if (setNonBlocking(socket_vector_[0]) == -1) {
		return -1;
	}
	if (setNonBlocking(socket_vector_[1]) == -1) {
		return -1;
	}
	return 0;
}

int Cgi::setupCgiRequestContext() {
	if (cgi_request_context_.setup() == -1) {
		return -1;
	}
	return 0;
}

} // namespace server
