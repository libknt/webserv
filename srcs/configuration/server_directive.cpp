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

int ServerDirective::parseServerDirective(std::vector<std::string>& tokens) {
	std::vector<std::string> location_tokens;
	std::vector<std::string> args;

	while (!tokens.empty()) {
		LocationDirective location_directive;
		std::string location_path;

		if (tokens.front() == "listen") {
			// listenの処理
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "________ listen ___________" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "server_name") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "________ server_name ___________" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "location") {
			// location の削除
			std::cout << "before location: " << tokens.front() << std::endl;
			tokens.erase(tokens.begin());
			std::cout << "after location: " << tokens.front()  << ": " << tokens[1] << std::endl;

			if (tokens.size()) {
				location_path = tokens.front();
				tokens.erase(tokens.begin());
				std::cout << "ああああああ: " << tokens.front() << std::endl;
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
