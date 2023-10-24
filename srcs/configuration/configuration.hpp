#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include "server_directive.hpp"

class Configuration {
private:
	ServerDirective server_;

	std::vector<std::string> tokenize_file_content(const std::string& path);
	std::vector<std::string> tokenize(std::string& line);
	bool isSpecialCharacter(const char& c);

public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int init(const std::string& path);
};


#endif
