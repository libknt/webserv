#include "cgi.hpp"

namespace server {

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_) {

	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		meta_variables_ = other.meta_variables_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
	}
	return *this;
}

Cgi::Cgi(const HttpRequest& request)
	: request_(request)
	, meta_variables_(request) {
	socket_vector_[0] = (-1);
	socket_vector_[1] = (-1);
}

int Cgi::setupInterProcessCommunication() {
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vector_) == -1) {
		std::cerr << "socketpair() failed(): " << strerror(errno) << std::endl;
		return -1;
	}
	if (hasBody()) {
		std::cout << "yesBody" << std::endl;
	} else {
		std::cout << "noBody" << std::endl;
	}
	return 0;
}

int Cgi::setup() {
	if (meta_variables_.setup() < 0) {
		std::cerr << "Cgi::setup(): meta_variables_.setup() failed" << std::endl;
		return -1;
	}
	if (setupInterProcessCommunication() < 0) {
		std::cerr << "Cgi::setupInterProcessCommunication() failed" << std::endl;
		return -1;
	}
	return 0;
}

bool Cgi::hasBody() const {
	return !request_.getBody().empty();
}

}