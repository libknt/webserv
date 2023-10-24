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

bool hasSpecialCharacter(const std::string& str) {
	std::vector<std::string> special_characters;
	special_characters.push_back("{");
	special_characters.push_back("}");
	special_characters.push_back(";");

	for (size_t i = 0; i < special_characters.size(); i++) {
		if (str.find(special_characters[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool isSpecialCharacter(const char& c) {
	return (c == '{' || c == '}' || c == ';');
}

void splitBySpecialCharacter(std::vector<std::string>& tokens, std::string& line) {
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
}

std::vector<std::string> Configuration::tokenize(const std::string& path) {
    std::ifstream conf_file(path);
    std::string line;
    std::vector<std::string> tokens;

    while (getline(conf_file, line)) {
        splitBySpecialCharacter(tokens, line);
    }

    return tokens;
}