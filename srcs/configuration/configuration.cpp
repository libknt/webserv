#include "configuration.hpp"

Configuration::Configuration() {}

Configuration::~Configuration() {}

Configuration::Configuration(const Configuration& other) {
	servers_ = other.servers_;
}

Configuration& Configuration::operator=(const Configuration& other) {
	if (this != &other) {
		servers_ = other.servers_;
	}
	return *this;
}

int Configuration::init(const std::string& path) {
	std::vector<std::string> tokens;
	tokens = tokenize_file_content(path);
	parseConfiguration(tokens);
	return 0;
}

bool Configuration::isSpecialCharacter(const char& c) {
	return (c == '{' || c == '}' || c == ';');
}

std::vector<std::string> Configuration::tokenize(std::string& line) {
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

std::vector<std::string> extractServerTokens(std::vector<std::string>& tokens) {
	std::vector<std::string> server_tokens;
	size_t i;
	int num_of_left_brace = 1;
	int num_of_right_brace = 0;

	// TODO: エラー処理
	if (tokens[0] != "server" || tokens[1] != "{") {
		std::cerr << "non first brace error" << std::endl;
		return server_tokens;
	}
	for (i = 2; i < tokens.size() && num_of_left_brace != num_of_right_brace; ++i) {
		if (tokens[i] == "{") {
			num_of_left_brace++;
		} else if (tokens[i] == "}") {
			num_of_right_brace++;
		} else {
			server_tokens.push_back(tokens[i]);
		}
	}
	if (num_of_left_brace != num_of_right_brace) {
		std::cerr << "non first brace error" << std::endl;
	}
	tokens.erase(tokens.begin(), tokens.begin() + i);
	return server_tokens;
}

int Configuration::parseConfiguration(std::vector<std::string>& tokens) {
	if (tokens.front() != "server") {
		return -1;
	}

	size_t i = 0;
	while (tokens.size() >= 3) {
		std::vector<std::string> server_tokens;
		if (tokens.front() == "server") {
			servers_.push_back(ServerDirective());
			server_tokens = extractServerTokens(tokens);
			servers_[i].parseServerDirective(server_tokens);
		}
		i++;
	}
	return 0;
}
