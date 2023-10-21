#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>
# include <iostream>
# include "location.hpp"

class Configuration {
private:
	int port_;
	std::string ip_address_;
	std::string server_name_;
	Location location_;
public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);
};

#endif