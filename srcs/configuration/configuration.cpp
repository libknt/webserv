#include "configuration.hpp"


Configuration::Configuration() {}

Configuration::~Configuration() {}

Configuration::Configuration(const Configuration& other) {
	port_ = other.port_;
	ip_address_ = other.ip_address_;
	server_name_ = other.server_name_;
	location_ = other.location_;
}

Configuration& Configuration::operator=(const Configuration& other) {
	if (this != &other) {
		port_ = other.port_;
		ip_address_ = other.ip_address_;
		server_name_ = other.server_name_;
		location_ = other.location_;
	}
	return *this;
}
