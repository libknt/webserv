#include "location_directive.hpp"

LocationDirective::LocationDirective() {}

LocationDirective::~LocationDirective() {}

LocationDirective::LocationDirective(const LocationDirective& other) {
	error_pages_ = other.error_pages_;
	client_max_body_size_ = other.client_max_body_size_;
	root_ = other.root_;
	index_ = other.index_;
	autoindex_ = other.autoindex_;
	allow_methods_ = other.allow_methods_;
	chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
}

LocationDirective& LocationDirective::operator=(const LocationDirective& other) {
	if (this != &other) {
		error_pages_ = other.error_pages_;
		client_max_body_size_ = other.client_max_body_size_;
		root_ = other.root_;
		index_ = other.index_;
		autoindex_ = other.autoindex_;
		allow_methods_ = other.allow_methods_;
		chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
	}
	return *this;
}

int LocationDirective::parseLocationDirective(std::string& location_path, std::vector<std::string>& tokens) {
	std::vector<std::string> args;
	(void)location_path;
	while (!tokens.empty()) {
		if (tokens.front() == "error_pages") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ error_pages ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "client_max_body_size") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ client_max_body_size ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "root") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ root ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "index") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ index ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "auto_index") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ auto_index ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "allow_methods") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ allow_methods ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else if (tokens.front() == "chunked_transfer_encoding") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ chuncked_trasfer_encoding ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
		} else {
			ParserUtils::printTokens(tokens);
			return -1;
		}
		args.clear();
	}
	return 0;
}