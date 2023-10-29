#ifndef SERVER_DIRECTIVE_HPP
#define SERVER_DIRECTIVE_HPP

#include "location_directive.hpp"
#include "parser_utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class ServerDirective {
private:
	std::string ip_address_;
	std::string port_;
	std::string server_name_;
	std::string default_error_page_;
	std::map<std::string, LocationDirective> locations_;

	int parseListenDirective(std::vector<std::string>& tokens);
	int parseServerNameDirective(std::vector<std::string>& tokens);
	int parseDefaultErrorPageDirective(std::vector<std::string>& tokens);

public:
	ServerDirective();
	~ServerDirective();
	ServerDirective(const ServerDirective& other);
	ServerDirective& operator=(const ServerDirective& other);

	int parseServerDirective(std::vector<std::string>& tokens);
	std::string getPort() const;
	std::string getIpAddress() const;
	std::string getServerName() const;
	std::string getDefaultErrorPage() const; 
	std::map<std::string, LocationDirective> getLocations() const;
};

std::ostream& operator<<(std::ostream& out, const ServerDirective& server_directive);

#endif