#include "cgi.hpp"

namespace server {

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& other)
	: request_(other.request_) {}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
	}
	return *this;
}

Cgi::Cgi(const HttpRequest& request)
	: request_(request) {
	std::cout << "Cgi::Cgi(const HttpRequest& request)" << std::endl;
}

}