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

int LocationDirective::parseLocationDirective(std::string& location_path, std::vector<std::string>& tokens) {
	std::vector<std::string> args;
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

int LocationDirective::parseErrorPageDirective(std::vector<std::string>& tokens) {
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i].size() < 2) {
			std::cerr << "Parse Error: parseErrorPageDirective" << std::endl;
			return -1;
		}
		for (size_t j = 0; j < tokens[j].size() - 1; ++j) {
			if (!std::isdigit(tokens[i][j])) {
				std::cerr << "Parse Error: parseErrorPageDirective" << std::endl;
				return -1;
			}
		}
	}
	return 0;
}

int LocationDirective::parseClientMaxBodySizeDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseClientMaxBodySizeDirective" << std::endl;
		return -1;
	}
	std::string token = tokens.front();
	if (token.size() < 2) {
		std::cerr << "Parse Error: parseClientMaxBodySizeDirective" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < token.size() - 1; ++i) {
		if (!std::isdigit(token[i])) {
			std::cerr << "Parse Error: parseClientMaxBodySizeDirective" << std::endl;
			return -1;
		}
	}
	if (token.back() != 'K' && token.back() != 'M') {
		std::cerr << "Parse Error: parseClientMaxBodySizeDirective" << std::endl;
		return -1;
	}
	return 0;
}

int LocationDirective::parseRootDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseRootDirective" << std::endl;
		return -1;
	}
	root_ = tokens.front();
	return 0;
}

int LocationDirective::parseIndexDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseIndexDirective" << std::endl;
		return -1;
	}
	index_ = tokens.front();
	return 0;
}

int LocationDirective::parseAutoindexDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseAutondexDirective" << std::endl;
		return -1;
	}
	if (tokens.front() != "on" && tokens.front() != "off") {
		std::cerr << "Parse Error: parseAutondexDirective" << std::endl;
		return -1;
	}
	autoindex_ = tokens.front();
	return 0;
}

int LocationDirective::parseAllowMethodsDirective(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseAllowMethodsDirective" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE") {
			std::cerr << "Parse Error: parseAllowMethodsDirective" << std::endl;
			return -1;
		}
		allow_methods_.push_back(tokens[i]);
	}
	return 0;
}

int LocationDirective::parseChunkedTransferEncodingDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseChunkedTransferEncodingDirective" << std::endl;
		return -1;
	}
	if (tokens.front() != "on" && tokens.front() != "off") {
		std::cerr << "Parse Error: parseChunkedTransferEncodingDirective" << std::endl;
		return -1;
	}
	chunked_transfer_encoding_ = tokens.front();
	return 0;
}

std::vector<std::string> LocationDirective::getErrorPage() const {
	return error_page_;
}

std::vector<std::string> LocationDirective::getAllowMethods() const {
	return allow_methods_;
}

std::string LocationDirective::getClientMaxBodySize() const {
	return client_max_body_size_;
}

std::string LocationDirective::getRoot() const {
	return root_;
}

std::string LocationDirective::getIndex() const {
	return index_;
}

std::string LocationDirective::getAutoindex() const {
	return autoindex_;
}

std::string LocationDirective::getChunkedTransferEncoding() const {
	return chunked_transfer_encoding_;
}

std::ostream& operator<<(std::ostream &out, const LocationDirective& location_directive) {
	std::vector<std::string> error_pages = location_directive.getErrorPage();
	for (size_t i = 0; i < error_pages.size(); ++i) {
		out << error_pages[i] << std::endl;
	}

	std::vector<std::string> allow_methods = location_directive.getAllowMethods();
	for (size_t i = 0; i < allow_methods.size(); ++i) {
		out << allow_methods[i] << std::endl;
	}

	out << location_directive.getClientMaxBodySize() << std::endl;
	out << location_directive.getRoot() << std::endl;
	out << location_directive.getIndex() << std::endl;
	out << location_directive.getAutoindex() << std::endl;
	out << location_directive.getChunkedTransferEncoding() << std::endl;

	return out;
}