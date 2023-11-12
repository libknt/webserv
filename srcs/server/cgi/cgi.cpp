#include "cgi.hpp"

namespace server {

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_) {}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		meta_variables_ = other.meta_variables_;
	}
	return *this;
}

Cgi::Cgi(const HttpRequest& request)
	: request_(request)
	, meta_variables_(request) {
	std::cout << "Cgi::Cgi(const HttpRequest& request)" << std::endl;
}

int Cgi::setup() {
	std::cout << "Cgi::setup()" << std::endl;
	if (meta_variables_.setup() < 0) {
		std::cerr << "Cgi::setup(): meta_variables_.setup() failed" << std::endl;
		return -1;
	}
	std::cout << "++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << meta_variables_ << std::endl;
	std::cout << "++++++++++++++++++++++++++++++++" << std::endl;
	return 0;
}

}