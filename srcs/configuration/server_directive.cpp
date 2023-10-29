#include "server_directive.hpp"

ServerDirective::ServerDirective() {
	ip_address_ = "127.0.0.1";
	port_ = "80";
	server_name_ = "localhost";
	default_error_page_ = "error_page.html";
}

ServerDirective::~ServerDirective() {}

ServerDirective::ServerDirective(const ServerDirective& other) {
	port_ = other.port_;
	ip_address_ = other.ip_address_;
	server_name_ = other.server_name_;
	default_error_page_ = other.default_error_page_;
	locations_ = other.locations_;
}

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

	while (!tokens.empty()) {
		LocationDirective location_directive;
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
			args = ParserUtils::extractTokensFromBlock(tokens);
			if (parseDefaultErrorPageDirective(args) == -1) {
				return -1;
			}
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
			if (location_directive.parseLocationDirective(location_tokens) == -1) {
				return -1;
			}
			locations_[location_path] = location_directive;
		} else {
			std::cout << "Parse Error: serverDirective" << std::endl;
			std::cout << tokens.front() << std::endl;
			return -1;
		}
		args.clear();
	}
	return 0;
}

int ServerDirective::parseDefaultErrorPageDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		return -1;
	}
	default_error_page_ = tokens.front();
	return 0;
}

int ServerDirective::parseListenDirective(std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::cerr << "Parse Error: parseListenDirective1" << std::endl;
		return -1;
	}

	std::string token = tokens.front();
	size_t found = token.find(":");
	if (found == std::string::npos || found == 0 || found == token.size() - 1) {
		std::cerr << "Parse Error: parseListenDirective2" << std::endl;
		return -1;
	}

	std::string port = token.substr(found + 1);
	for (size_t i = 0; i < port.size(); ++i) {
		if (!isdigit(port[i])) {
			std::cerr << "Parse Error: parseListenDirective3" << std::endl;
			return -1;
		}
	}

	ip_address_ = tokens.front();
	port_ = tokens.back();
	return 0;
}

int ServerDirective::parseServerNameDirective(std::vector<std::string>& tokens) {
	if (tokens.size() != 1) {
		std::cerr << "Parse Error: parseServerNameDirective" << std::endl;
		return -1;
	}
	server_name_ = tokens.front();
	return 0;
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

std::map<std::string, LocationDirective> ServerDirective::getLocations() const {
	return locations_;
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
		out << "LocationPath: " << it->first << std::endl;
		out << it->second << std::endl;
		i++;
	}
	return out;
}