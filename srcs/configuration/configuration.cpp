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

int Configuration::init(const std::string& path) {
	std::vector<std::string> tokens;
	tokens = tokenize(path);
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << tokens[i] << std::endl;
	}
	return 0;
}

bool Configuration::isSpecialCharactor(const char& c) {
	return (c == '{' || c == '}' || c == ';');
}

std::vector<std::string> Configuration::tokenize(const std::string& path) {
	std::ifstream conf_file(path);
	std::string line;
	std::vector<std::string> tokens;

	while (getline(conf_file, line)) {
		std::stringstream stringstream(line);
		std::string word;
		while (stringstream >> word) {
			tokens.push_back(word);
		}

	}
	return tokens;
}