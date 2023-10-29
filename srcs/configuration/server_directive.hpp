#ifndef SERVER_DIRECTIVE_HPP
# define SERVER_DIRECTIVE_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <map>
# include <sstream>
# include "location_directive.hpp"
# include "parser_utils.hpp"

class ServerDirective {
private:
	std::string ip_address_;
	std::string port_;
	std::string server_name_;
	std::map<std::string, LocationDirective> locations_;

	int parseListenDirective(std::vector<std::string>& tokens);
	int parseServerNameDirective(std::vector<std::string>& tokens);
public:
	ServerDirective();
	~ServerDirective();
	ServerDirective(const ServerDirective& other);
	ServerDirective& operator=(const ServerDirective& other);

	int parseServerDirective(std::vector<std::string>& tokens);
	std::string getPort() const;
	std::string getIpAddress() const;
	std::string getServerName() const;
	std::map<std::string, LocationDirective> getLocations() const;
};

std::ostream& operator<<(std::ostream &out, const ServerDirective& server_directive);

#endif