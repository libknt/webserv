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
	std::vector<ServerDirective> servers_;

	std::vector<std::string> tokenize_file_content(const std::string& path);
	std::vector<std::string> tokenize(std::string& line);
	bool isSpecialCharacter(const char& c);
	int parseConfiguration(std::vector<std::string>& tokens);

public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int init(const std::string& path);
};


#endif
