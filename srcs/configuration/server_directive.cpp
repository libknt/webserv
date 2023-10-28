#include "server_directive.hpp"

ServerDirective::ServerDirective() {}

ServerDirective::~ServerDirective() {}

ServerDirective::ServerDirective(const ServerDirective& other) {
	port_ = other.port_;
	ip_address_ = other.ip_address_;
	server_name_ = other.server_name_;
	locations_ = other.locations_;
}

ServerDirective& ServerDirective::operator=(const ServerDirective& other) {
	if (this != &other) {
		port_ = other.port_;
		ip_address_ = other.ip_address_;
		server_name_ = other.server_name_;
		locations_ = other.locations_;
	}
	return *this;
}

int ServerDirective::parseServerDirective(std::list<std::string>& tokens) {
	std::list<std::string> location_tokens;
	std::list<std::string> args;

	while (!tokens.empty()) {
		LocationDirective location_directive;
		std::string location_path;

		if (tokens.front() == "listen") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "________ listen ___________" << std::endl;
			ParserUtils::printTokens(args);
			parseListenDirective(args);
		} else if (tokens.front() == "server_name") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "________ server_name ___________" << std::endl;
			ParserUtils::printTokens(args);
			parseServerNameDirective(args);
		} else if (tokens.front() == "location") {
			tokens.erase(tokens.begin());
			if (tokens.size()) {
				location_path = tokens.front();
				tokens.erase(tokens.begin());
			} else {
				std::cerr << "Parse Error: Invalid Location" << std::endl;
				return -1;
			}
			location_tokens = ParserUtils::extractTokensFromBlock(tokens);
			location_directive.parseLocationDirective(location_path, location_tokens);
		} else {
			std::cout << "Parse Error: serverDirective" << std::endl;
			std::cout << tokens.front() << std::endl;
			return -1;
		}
		args.clear();
	}
	return 0;
}

// エラー処理
int ServerDirective::parseListenDirective(std::list<std::string>& tokens) {
	int port;

	if (tokens.size() == 1) {
		token >> port;
		port_ = port;
	} else if (tokens.size() == 3) {
		ip_address_ = tokens.front();
		port_ = (tokens.front());
	} else {
		return -1;
	}
	return 0;
}

int ServerDirective::parseServerNameDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}

// listen <port>;

// listen <ipaddress>:<port>;
