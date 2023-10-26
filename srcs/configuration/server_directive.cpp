#include "server_directive.hpp"

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

std::vector<std::string> extractLocationTokens(std::vector<std::string>& tokens) {
	std::vector<std::string> location_tokens;
	size_t i;
	int num_of_left_brace = 1;
	int num_of_right_brace = 0;

	// TODO: エラー処理
	if (tokens[0] != "location" || tokens[1] != "{") {
		std::cerr << "non first brace error" << std::endl;
		return location_tokens;
	}
	for (i = 2; i < tokens.size() && num_of_left_brace != num_of_right_brace; ++i) {
		if (tokens[i] == "{") {
			num_of_left_brace++;
		} else if (tokens[i] == "}") {
			num_of_right_brace++;
		} else {
			location_tokens.push_back(tokens[i]);
		}
	}
	if (num_of_left_brace != num_of_right_brace) {
		std::cerr << "non first brace error" << std::endl;
	}
	tokens.erase(tokens.begin(), tokens.begin() + i);
	return location_tokens;
}

int ServerDirective::parseServerDirective(std::vector<std::string>& tokens) {
	std::vector<std::string> location_tokens;
	std::vector<std::string> args;
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i] == "listen") {
			// listenの処理
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
		} else if (tokens[i] == "server_name") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			// server_nameの処理
		} else if (tokens[i] == "location") {
			// locationの処理
			location_tokens = extractLocationTokens(tokens);
			location_.parseLocationDirective(location_tokens);
		} else {
			// error
			std::cout << "parseServerDirective error: " << tokens[i] << std::endl;
			return -1;
		}
	}
	return 0;
}
