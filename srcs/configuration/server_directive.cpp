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
			tokens.erase(tokens.begin());
			location_tokens = ParserUtils::extractTokensFromBlock(tokens);
			location_.parseLocationDirective(location_tokens);
		} else {
			// error
			std::cout << "parseServerDirective error: " << tokens[i] << std::endl;
			return -1;
		}
	}
	return 0;
}
