#include "location_directive.hpp"

LocationDirective::LocationDirective() {
	std::vector<std::string> allow_methods;
	allow_methods.push_back("GET");
	allow_methods_ = allow_methods;
	client_max_body_size_ = "1M";
	root_ = "html";
	index_ = "index.html";
	autoindex_ = "off";
	chunked_transfer_encoding_ = "off";
	cgi_ = "off";
}

LocationDirective::~LocationDirective() {}

LocationDirective::LocationDirective(const LocationDirective& other)
	: error_page_(other.error_page_)
	, allow_methods_(other.allow_methods_)
	, client_max_body_size_(other.client_max_body_size_)
	, root_(other.root_)
	, index_(other.index_)
	, autoindex_(other.autoindex_)
	, chunked_transfer_encoding_(other.chunked_transfer_encoding_)
	, cgi_(other.cgi_)
	, cgi_extensions_(other.cgi_extensions_) {}

LocationDirective& LocationDirective::operator=(const LocationDirective& other) {
	if (this != &other) {
		error_page_ = other.error_page_;
		allow_methods_ = other.allow_methods_;
		client_max_body_size_ = other.client_max_body_size_;
		root_ = other.root_;
		index_ = other.index_;
		autoindex_ = other.autoindex_;
		chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
		cgi_ = other.cgi_;
		cgi_extensions_ = other.cgi_extensions_;
	}
	return *this;
}

int LocationDirective::parseLocationDirective(std::vector<std::string>& tokens) {
	std::vector<std::string> args;
	while (!tokens.empty()) {
		if (tokens.front() == "error_page") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseErrorPageDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "client_max_body_size") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseClientMaxBodySizeDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "root") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseRootDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "index") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseIndexDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "autoindex") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseAutoindexDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "allow_methods") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseAllowMethodsDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "chunked_transfer_encoding") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseChunkedTransferEncodingDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "cgi") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseCgiDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "cgi_extensions") {
			cgi_extensions_.clear();
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseCgiExtensionsDirective(args) == -1) {
				return -1;
			}
		} else {
			std::cerr << "Parse Error: Location Directive" << std::endl;
			return -1;
		}
		args.clear();
	}
	return 0;
}

int LocationDirective::parseErrorPageDirective(std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
		std::cerr << "Parse Error: parseErrorPageDirective1" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < tokens.size() - 1; ++i) {
		for (size_t j = 0; j < tokens[i].size(); ++j) {
			if (!std::isdigit(tokens[i][j])) {
				std::cout << tokens[i][j] << std::endl;
				std::cerr << "Parse Error: parseErrorPageDirective2" << std::endl;
				return -1;
			}
		}
		error_page_.push_back(tokens[i]);
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
	if (token[token.size() - 1] != 'K' && token[token.size() - 1] != 'M') {
		std::cerr << "Parse Error: parseClientMaxBodySizeDirective" << std::endl;
		return -1;
	}
	client_max_body_size_ = tokens.front();
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
		std::cerr << "Parse Error: parseAutoindexDirective" << std::endl;
		return -1;
	}
	if (tokens.front() != "on" && tokens.front() != "off") {
		std::cerr << "Parse Error: parseAutoindexDirective" << std::endl;
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

int LocationDirective::parseCgiDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseCgiDirective" << std::endl;
		return -1;
	}
	if (tokens.front() != "on" && tokens.front() != "off") {
		std::cerr << "Parse Error: parseCgiDirective" << std::endl;
		return -1;
	}
	cgi_ = tokens.front();
	return 0;
}

int LocationDirective::parseCgiExtensionsDirective(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i] != ".cgi" && tokens[i] != ".pl" && tokens[i] != ".py" && tokens[i] != ".php" && tokens[i] != ".sh") {
			std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
			return -1;
		}
		cgi_extensions_.push_back(tokens[i]);
	}
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

std::string LocationDirective::getCgi() const {
	return cgi_;
}

std::vector<std::string> LocationDirective::getCgiExtensions() const {
	return cgi_extensions_;
}

std::ostream& operator<<(std::ostream& out, const LocationDirective& location_directive) {
	std::vector<std::string> error_pages = location_directive.getErrorPage();
	out << "ErrorPages: " << std::endl;
	for (size_t i = 0; i < error_pages.size(); ++i) {
		out << error_pages[i] << std::endl;
	}

	std::vector<std::string> allow_methods = location_directive.getAllowMethods();
	out << "HTTPMethods: " << std::endl;
	for (size_t i = 0; i < allow_methods.size(); ++i) {
		out << allow_methods[i] << std::endl;
	}

	out << "GetClientMaxBodySize: " << location_directive.getClientMaxBodySize() << std::endl;
	out << "Root: " << location_directive.getRoot() << std::endl;
	out << "Index: " << location_directive.getIndex() << std::endl;
	out << "AutoIndex: " << location_directive.getAutoindex() << std::endl;
	out << "ChuckedTransferEncoding: " << location_directive.getChunkedTransferEncoding() << std::endl;
	out << "CGI: " << location_directive.getCgi() << std::endl;

	std::vector<std::string> cgi_extensions = location_directive.getCgiExtensions();
	out << "CGIExtensions: ";
	for (size_t i = 0; i < cgi_extensions.size(); ++i) {
		out << cgi_extensions[i] << ", ";
	}
	out << std::endl;

	return out;
}