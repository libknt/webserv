#include "server_directive.hpp"

ServerDirective::ServerDirective()
	: ip_address_("127.0.0.1")
	, port_("80")
	, server_name_("localhost")
	, default_error_page_("error_page.html") {}

ServerDirective::~ServerDirective() {}

ServerDirective::ServerDirective(const ServerDirective& other)
	: ip_address_(other.ip_address_)
	, port_(other.port_)
	, server_name_(other.server_name_)
	, default_error_page_(other.default_error_page_)
	, locations_(other.locations_) {}

ServerDirective& ServerDirective::operator=(const ServerDirective& other) {
	if (this != &other) {
		port_ = other.port_;
		ip_address_ = other.ip_address_;
		server_name_ = other.server_name_;
		default_error_page_ = other.default_error_page_;
		locations_ = other.locations_;
	}
	return *this;
}

int ServerDirective::parseServerDirective(std::vector<std::string>& tokens) {
	std::vector<std::string> location_tokens;
	std::vector<std::string> args;

	// TODO: too many elseif
	while (!tokens.empty()) {
		std::string location_path;

		if (tokens.front() == "listen") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseListenDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "server_name") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseServerNameDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "default_error_page") {
			args = ParserUtils::extractTokensUntilSemicolon(tokens);
			if (parseDefaultErrorPageDirective(args) == -1) {
				return -1;
			}
		} else if (tokens.front() == "location") {
			tokens.erase(tokens.begin());
			location_path = parseLocationPath(tokens);
			if (location_path.empty()) {
				return -1;
			}
			LocationDirective location_directive(location_path);
			location_tokens = ParserUtils::extractTokensFromBlock(tokens);
			if (location_directive.parseLocationDirective(location_tokens) == -1) {
				return -1;
			}
			if (locations_.find(location_path) == locations_.end()) {
				locations_[location_path] = location_directive;
			}
		} else {
			std::cerr << "Parse Error: serverDirective" << std::endl;
			return -1;
		}
		args.clear();
	}
	if (locations_.find("/") == locations_.end()) {
		locations_["/"] = LocationDirective();
	}
	return 0;
}

int ServerDirective::parseListenDirective(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseListenDirective" << std::endl;
		return -1;
	}

	std::string token = tokens.front();
	size_t found = token.find(":");
	if (found == std::string::npos || found == 0 || found == token.size() - 1) {
		std::cerr << "Parse Error: parseListenDirective" << std::endl;
		return -1;
	}

	ip_address_ = token.substr(0, found);
	if (!isValidIPv4(ip_address_)) {
		std::cerr << "Parse Error: Invalid IP address" << std::endl;
		return -1;
	}

	port_ = token.substr(found + 1);
	if (!isValidPort(port_)) {
		std::cerr << "Parse Error:Invalid port" << std::endl;
		return -1;
	}
	return 0;
}

bool ServerDirective::isValidIPv4(const std::string& ip_address) const {
	std::stringstream stringstream(ip_address);
	int segment1, segment2, segment3, segment4;
	char dot1, dot2, dot3;

	stringstream >> segment1 >> dot1 >> segment2 >> dot2 >> segment3 >> dot3 >> segment4;
	if (stringstream.fail() || !stringstream.eof()) {
		return false;
	}
	return dot1 == '.' && dot2 == '.' && dot3 == '.' && isValidIPSegment(segment1) &&
		   isValidIPSegment(segment2) && isValidIPSegment(segment3) && isValidIPSegment(segment4);
}

bool ServerDirective::isValidIPSegment(int num) const {
	return 0 <= num && num <= 255;
}

bool ServerDirective::isValidPort(const std::string& port_string) {
	std::stringstream stringstream(port_string);
	int port_number;
	const int max_port = 65535;
	const int min_port = 0;

	stringstream >> port_number;
	if (stringstream.fail() || !stringstream.eof()) {
		return false;
	}
	return port_number >= min_port && port_number <= max_port;
}

int ServerDirective::parseServerNameDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseServerNameDirective" << std::endl;
		return -1;
	}
	server_name_ = tokens.front();
	return 0;
}

int ServerDirective::parseDefaultErrorPageDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseDefaultErrorPageDirective" << std::endl;
		return -1;
	}
	default_error_page_ = tokens.front();
	return 0;
}

std::string ServerDirective::parseLocationPath(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseLocationPath" << std::endl;
		return std::string();
	}

	std::string token = tokens.front();
	if (token[0] != '/') {
		std::cerr << "Parse Error: parseLocationPath" << std::endl;
		return std::string();
	}

	bool is_last_char_slash = false;
	std::string location_path;
	for (size_t i = 0; i < token.size(); ++i) {
		if (token[i] == '/') {
			if (!is_last_char_slash) {
				location_path += token[i];
				is_last_char_slash = true;
			}
		} else {
			location_path += token[i];
			is_last_char_slash = false;
		}
	}

	tokens.erase(tokens.begin());
	return location_path;
}

std::string ServerDirective::getPort() const {
	return port_;
}

std::string ServerDirective::getIpAddress() const {
	return ip_address_;
}

std::string ServerDirective::getServerName() const {
	return server_name_;
}

std::string ServerDirective::getDefaultErrorPage() const {
	return default_error_page_;
}

const std::map<std::string, LocationDirective>& ServerDirective::getLocations() const {
	return locations_;
}

LocationDirective& ServerDirective::findLocation(std::string request_path) {
	for (size_t i = 0; i < request_path.size(); ++i) {
		std::string path = request_path.substr(0, request_path.size() - i);
		if (locations_.count(path)) {
			return locations_[path];
		}
	}
	return locations_["/"];
}

std::ostream& operator<<(std::ostream& out, const ServerDirective& server_directive) {
	out << "IPAddress: " << server_directive.getIpAddress() << std::endl;
	out << "Port: " << server_directive.getPort() << std::endl;
	out << "ServerName: " << server_directive.getServerName() << std::endl;
	out << "DefaultErrorPage: " << server_directive.getDefaultErrorPage() << std::endl;

	std::map<std::string, LocationDirective> locations = server_directive.getLocations();
	size_t i = 0;
	for (std::map<std::string, LocationDirective>::iterator it = locations.begin();
		 it != locations.end();
		 ++it) {
		out << "===== location" << i << " =====" << std::endl;
		out << "Location: " << it->first << std::endl;
		out << it->second << std::endl;
		i++;
	}
	return out;
}