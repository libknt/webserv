#ifndef SERVER_DIRECTIVE_HPP
# define SERVER_DIRECTIVE_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <map>
# include "location_directive.hpp"
# include "parser_utils.hpp"

class ServerDirective {
private:
	int port_;
	std::string ip_address_;
	std::string server_name_;
	std::map<std::string, LocationDirective> locations_;

	int parseListenDirective(std::list<std::string>& tokens);
	int parseServerNameDirective(std::list<std::string>& tokens);
public:
	ServerDirective();
	~ServerDirective();
	ServerDirective(const ServerDirective& other);
	ServerDirective& operator=(const ServerDirective& other);

	int parseServerDirective(std::list<std::string>& tokens);
};

#endif