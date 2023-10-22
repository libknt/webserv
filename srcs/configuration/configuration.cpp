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

Configuration& parseConfiguration(Configuration& configuration, std::string path) {
	std::ifstream conf_file(path);
	std::string line;

	while (getline(conf_file, line)) {
		for (size_t i = 0; i < line.size(); i++) {
			// スペースを読み飛ばす
			while (isspace(line[i]))
				i++;
			// word単位に分ける
			// 文法が正しいか判断する(directiveによって文法が違うから持っておく必要がある)
		}
	}
	return configuration;
}