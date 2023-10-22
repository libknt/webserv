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

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
    if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

// tokenize, parseに分けても良さそうな気がしてきた
Configuration& parseConfiguration(Configuration& configuration, std::string path) {
	std::ifstream conf_file(path);
	std::string line;
	std::vector<std::string> words;

	while (getline(conf_file, line)) {
		// スペースを飛ばしながら単語に分ける
		std::stringstream stringstream(line);
		std::string word;
		while (stringstream >> word) {
			words.push_back(word);
		}
		// 文法が正しいか判断する(directiveによって文法が違うから持っておく必要がある)
	}
	for (size_t i = 0; i < words.size(); i++) {
		std::cout << words[i] << std::endl;
	}
	return configuration;
}