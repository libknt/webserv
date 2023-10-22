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


int ServerDirective::setConfiguration(const std::string& path) {
	parseConfiguration()
	return 0;
}

// tokenize, parseに分けても良さそうな気がしてきた
ServerDirective& parseConfiguration(ServerDirective& configuration, std::string path) {
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