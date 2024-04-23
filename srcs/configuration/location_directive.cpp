#include "location_directive.hpp"

LocationDirective::LocationDirective()
	: location_path_("/")
	, root_("html")
	, index_("index.html")
	, autoindex_(false)
	, chunked_transfer_encoding_(false)
	, cgi_(false) {
	std::set<std::string> allow_methods;
	allow_methods.insert("GET");
	allow_methods_ = allow_methods;
}

LocationDirective::LocationDirective(const std::string& location_path)
	: location_path_(location_path)
	, root_("html")
	, index_("index.html")
	, autoindex_(false)
	, chunked_transfer_encoding_(false)
	, cgi_(false) {
	std::set<std::string> allow_methods;
	allow_methods.insert("GET");
	allow_methods_ = allow_methods;
}

LocationDirective::~LocationDirective() {}

LocationDirective::LocationDirective(const LocationDirective& other)
	: location_path_(other.location_path_)
	, default_error_page_(other.default_error_page_)
	, error_pages_(other.error_pages_)
	, allow_methods_(other.allow_methods_)
	, root_(other.root_)
	, index_(other.index_)
	, autoindex_(other.autoindex_)
	, return_(other.return_)
	, chunked_transfer_encoding_(other.chunked_transfer_encoding_)
	, cgi_(other.cgi_)
	, cgi_extensions_(other.cgi_extensions_) {}

LocationDirective& LocationDirective::operator=(const LocationDirective& other) {
	if (this != &other) {
		default_error_page_ = other.default_error_page_;
		error_pages_ = other.error_pages_;
		allow_methods_ = other.allow_methods_;
		root_ = other.root_;
		index_ = other.index_;
		autoindex_ = other.autoindex_;
		return_ = other.return_;
		chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
		cgi_ = other.cgi_;
		cgi_extensions_ = other.cgi_extensions_;
	}
	return *this;
}

int LocationDirective::parseLocationDirective(std::vector<std::string>& tokens) {
	std::vector<std::string> args;
	while (!tokens.empty()) {
		if (tokens.front() == "default_error_page") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseDefaultErrorPageDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "error_page") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseErrorPageDirective(args) == -1) {
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
		} else if (tokens.front() == "return") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseReturnDirective(args) == -1) {
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

int LocationDirective::parseDefaultErrorPageDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseDefaultErrorPageDirective" << std::endl;
		return -1;
	}
	default_error_page_ = tokens.front();
	return 0;
}

int LocationDirective::parseErrorPageDirective(std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
		std::cerr << "Parse Error: parseErrorPageDirective1" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < tokens.size() - 1; ++i) {
		if (!isStatusCode(tokens[i])) {
			std::cerr << "Parse Error: parseErrorPageDirective1" << std::endl;
			return -1;
		}
		error_pages_.insert(std::make_pair(tokens[i], tokens[tokens.size() - 1]));
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
		std::cerr << "Parse Error: parseAutoindexDirective" << std::endl;
		return -1;
	}
	if (tokens.front() == "on") {
		autoindex_ = true;
	} else if (tokens.front() == "off") {
		autoindex_ = false;
	} else {
		std::cerr << "Parse Error: parseAutoindexDirective" << std::endl;
		return -1;
	}
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
		allow_methods_.insert(tokens[i]);
	}
	return 0;
}

int LocationDirective::parseReturnDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseReturnDirective" << std::endl;
		return -1;
	}
	return_ = tokens[0];
	return 0;
}

int LocationDirective::parseChunkedTransferEncodingDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseChunkedTransferEncodingDirective" << std::endl;
		return -1;
	}
	if (tokens.front() == "on") {
		chunked_transfer_encoding_ = true;
	} else if (tokens.front() == "off") {
		chunked_transfer_encoding_ = false;
	} else {
		std::cerr << "Parse Error: parseChunkedTransferEncodingDirective" << std::endl;
		return -1;
	}
	return 0;
}

std::string LocationDirective::getDefaultErrorPage() const {
	return default_error_page_;
}

std::map<std::string, std::string> LocationDirective::getErrorPages() const {
	return error_pages_;
}

std::string const LocationDirective::findErrorPagePath(
	http_status_code::STATUS_CODE status_code) const {
	std::stringstream stringstream;
	stringstream << status_code;
	std::map<std::string, std::string>::const_iterator it = error_pages_.find(stringstream.str());
	return it != error_pages_.end() ? it->second : "";
}

int LocationDirective::parseCgiDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseCgiDirective" << std::endl;
		return -1;
	}
	if (tokens.front() == "on") {
		cgi_ = true;
	} else if (tokens.front() == "off") {
		cgi_ = false;
	} else {
		std::cerr << "Parse Error: parseCgiDirective" << std::endl;
		return -1;
	}
	return 0;
}

int LocationDirective::parseCgiExtensionsDirective(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
		return -1;
	}
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i].size() < 2) {
			std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
			return -1;
		}
		for (size_t j = 0; j < tokens[i].size(); ++j) {
			if (j == 0) {
				if (tokens[i][j] != '.') {
					std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
					return -1;
				}
			} else if (!std::isalnum(tokens[i][j])) {
				std::cerr << "Parse Error: parseCgiExtensionsDirective" << std::endl;
				std::cerr << tokens[i][j] << std::endl;
				return -1;
			}
		}
		cgi_extensions_.insert(tokens[i]);
	}
	return 0;
}

std::string LocationDirective::getLocationPath() const {
	return location_path_;
}

std::set<std::string> LocationDirective::getAllowMethods() const {
	return allow_methods_;
}

std::string LocationDirective::getRoot() const {
	return root_;
}

std::string LocationDirective::getIndex() const {
	return index_;
}

bool LocationDirective::getAutoindex() const {
	return autoindex_;
}

std::string const& LocationDirective::getReturn() const {
	return return_;
}

bool LocationDirective::getChunkedTransferEncoding() const {
	return chunked_transfer_encoding_;
}

bool LocationDirective::getCgi() const {
	return cgi_;
}

const std::set<std::string>& LocationDirective::getCgiExtensions() const {
	return cgi_extensions_;
}

bool LocationDirective::isAllowMethod(const std::string& method) const {
	if (allow_methods_.count(method)) {
		return true;
	}
	return false;
}

bool LocationDirective::isCgiExtension(const std::string& extension) const {
	if (cgi_extensions_.count(extension)) {
		return true;
	}
	return false;
}

bool LocationDirective::isStatusCode(const std::string& status_code) {
	if (status_code.size() > 3) {
		return false;
	}
	for (size_t i = 0; i < status_code.size(); ++i) {
		if (!std::isdigit(status_code[i])) {
			return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& out, const LocationDirective& location_directive) {
	out << "LocationPath: " << location_directive.getLocationPath() << std::endl;
	out << "DefaultErrorPage: " << location_directive.getDefaultErrorPage() << std::endl;
	std::map<std::string, std::string> error_pages = location_directive.getErrorPages();
	out << "ErrorPages: " << std::endl;
	for (std::map<std::string, std::string>::iterator it = error_pages.begin();
		 it != error_pages.end();
		 ++it) {
		out << it->first << ":" << it->second << ", ";
	}
	out << std::endl;

	std::set<std::string> allow_methods = location_directive.getAllowMethods();
	out << "HTTPMethods: " << std::endl;
	for (std::set<std::string>::iterator it = allow_methods.begin(); it != allow_methods.end();
		 ++it) {
		out << *it << ", ";
	}
	out << std::endl;

	out << "Root: " << location_directive.getRoot() << std::endl;
	out << "Index: " << location_directive.getIndex() << std::endl;
	out << "AutoIndex: " << location_directive.getAutoindex() << std::endl;

	std::string return_directive = location_directive.getReturn();
	if (return_directive.size()) {
		out << "Return: " << return_directive << std::endl;
	}

	out << "ChuckedTransferEncoding: " << location_directive.getChunkedTransferEncoding()
		<< std::endl;
	out << "CGI: " << location_directive.getCgi() << std::endl;

	std::set<std::string> cgi_extensions = location_directive.getCgiExtensions();
	out << "CGIExtensions: ";
	for (std::set<std::string>::iterator it = cgi_extensions.begin(); it != cgi_extensions.end();
		 ++it) {
		out << *it << ", ";
	}
	out << std::endl;

	return out;
}
