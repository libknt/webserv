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
	tokens = tokenize_file_content(path);
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << tokens[i] << std::endl;
	}
	return 0;
}

bool isSpecialCharacter(const char& c) {
	return (c == '{' || c == '}' || c == ';');
}

std::vector<std::string> tokenize(std::string& line) {
	std::vector<std::string> tokens;
    std::string token;

    for (size_t i = 0; i < line.size(); ++i) {
        if (isspace(line[i]) || isSpecialCharacter(line[i])) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            if (isSpecialCharacter(line[i])) {
                tokens.push_back(std::string(1, line[i]));
            }
        } else {
            token += line[i];
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
	return tokens;
}

 std::vector<std::string> Configuration::tokenize_file_content(const std::string& path) {
    std::ifstream conf_file(path);
    std::string line;
    std::vector<std::string> tokens;

    while (getline(conf_file, line)) {
		std::vector<std::string> tmp = tokenize(line);
		tokens.insert(tokens.end(), tmp.begin(), tmp.end());
    }

    return tokens;
}