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

	std::vector<std::string> tokenize(const std::string& path);
public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int init(const std::string& path);
};


#endif
