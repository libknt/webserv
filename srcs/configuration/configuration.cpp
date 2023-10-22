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
	std::vector<std::string> tokens;
	tokens = tokenize(path);
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << tokens[i] << std::endl;
	}
	return 0;
}

std::vector<std::string> Configuration::tokenize(const std::string& path) {
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
	}
	return words;
}