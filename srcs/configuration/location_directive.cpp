#include "location_directive.hpp"

LocationDirective::LocationDirective() {}

LocationDirective::~LocationDirective() {}

LocationDirective::LocationDirective(const LocationDirective& other) {
	error_page_ = other.error_page_;
	client_max_body_size_ = other.client_max_body_size_;
	root_ = other.root_;
	index_ = other.index_;
	autoindex_ = other.autoindex_;
	allow_methods_ = other.allow_methods_;
	chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
}

LocationDirective& LocationDirective::operator=(const LocationDirective& other) {
	if (this != &other) {
		error_page_ = other.error_page_;
		client_max_body_size_ = other.client_max_body_size_;
		root_ = other.root_;
		index_ = other.index_;
		autoindex_ = other.autoindex_;
		allow_methods_ = other.allow_methods_;
		chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
	}
	return *this;
}

int LocationDirective::parseLocationDirective(std::string& location_path, std::list<std::string>& tokens) {
	std::list<std::string> args;
	(void)location_path;
	while (!tokens.empty()) {
		if (tokens.front() == "error_page") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ error_page ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseErrorPageDirective(args);
		} else if (tokens.front() == "client_max_body_size") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ client_max_body_size ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseClientMaxBodySizeDirective(args);
		} else if (tokens.front() == "root") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ root ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseRootDirective(args);
		} else if (tokens.front() == "index") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ index ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseIndexDirective(args);
		} else if (tokens.front() == "autoindex") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ autoindex ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseAutoindexDirective(args);
		} else if (tokens.front() == "allow_methods") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ allow_methods ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseAllowMethodsDirective(args);
		} else if (tokens.front() == "chunked_transfer_encoding") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			std::cout << "+++++++++ chuncked_transfer_encoding ++++++++++++" << std::endl;
			ParserUtils::printTokens(args);
			parseChunkedTransferEncodingDirective(args);
		} else {
			std::cout << "Parse Error: Location Directive" << std::endl;
			ParserUtils::printTokens(tokens);
			return -1;
		}
		args.clear();
	}
	ParserUtils::printTokens(tokens);
	return 0;
}

int LocationDirective::parseErrorPageDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}

int LocationDirective::parseClientMaxBodySizeDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}

int LocationDirective::parseRootDirective(std::list<std::string>& tokens) {
	if (tokens.size() != 1) {
		return -1;
	}
	root_ = tokens.front();
	return 0;
}

int LocationDirective::parseIndexDirective(std::list<std::string>& tokens) {
	if (tokens.size() != 1) {
		return -1;
	}
	index_ = tokens.front();
	return 0;
}

int LocationDirective::parseAutoindexDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}

int LocationDirective::parseAllowMethodsDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}

int LocationDirective::parseChunkedTransferEncodingDirective(std::list<std::string>& tokens) {
	(void)tokens;
	return 0;
}
