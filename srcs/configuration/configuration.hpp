#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include "server_directive.hpp"
# include "parser_utils.hpp"

class Configuration {
private:
	std::list<ServerDirective> servers_;

	std::list<std::string> tokenize_file_content(const std::string& path);
	std::list<std::string> tokenize(std::string& line);
	bool isSpecialCharacter(const char& c);
	int parseConfiguration(std::list<std::string>& tokens);

public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int init(const std::string& path);
};


#endif
