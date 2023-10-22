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
public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int setConfiguration(const std::string& path);
};

Configuration& parseConfiguration(Configuration& configuration, std::string path);

#endif