#include "cgi.hpp"

namespace server {

Cgi::Cgi() {}

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& other) {
	(void)other;
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
	}
	return *this;
}

}