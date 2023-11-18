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
	std::map<std::string, LocationDirective> locations_;

	int parseListenDirective(std::vector<std::string>& tokens);
	int parseServerNameDirective(std::vector<std::string>& tokens);
	std::string parseLocationPath(std::vector<std::string>& tokens);
	bool isValidIPSegment(int num) const;
	bool isValidIPv4(const std::string& ip) const;
	bool isValidPort(const std::string& port_string);

public:
	ServerDirective();
	~ServerDirective();
	ServerDirective(const ServerDirective& other);
	ServerDirective& operator=(const ServerDirective& other);

	int parseServerDirective(std::vector<std::string>& tokens);
	std::string getPort() const;
	std::string getIpAddress() const;
	std::string getServerName() const;
	const std::map<std::string, LocationDirective>& getLocations() const;
	bool isCgiLocation(const std::string& location, const std::string& script_file_name) const;
};

std::ostream& operator<<(std::ostream& out, const ServerDirective& server_directive);

#endif
