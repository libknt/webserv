#include "configuration.hpp"

Configuration::Configuration() {}

Configuration::~Configuration() {}

Configuration::Configuration(const Configuration& other) {
	server_ = other.server_;
}

Configuration& Configuration::operator=(const Configuration& other) {
	if (this != &other) {
		server_ = other.server_;
	}
	return *this;
}


int Configuration::setConfiguration(const std::string& path) {
	parseConfiguration()
	return 0;
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