#include "configuration.hpp"

ServerDirective::ServerDirective() {}

ServerDirective::~ServerDirective() {}

ServerDirective::ServerDirective(const ServerDirective& other) {
	port_ = other.port_;
	ip_address_ = other.ip_address_;
	server_name_ = other.server_name_;
	location_ = other.location_;
}

ServerDirective& ServerDirective::operator=(const ServerDirective& other) {
	if (this != &other) {
		port_ = other.port_;
		ip_address_ = other.ip_address_;
		server_name_ = other.server_name_;
		location_ = other.location_;
	}
	return *this;
}

int ServerDirective::parseServerDirective(const std::vector<std::string>& tokens) {
	std::cout << tokens.at(0) << std::endl;
	return 0;
}
