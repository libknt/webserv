#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "parser_utils.hpp"
#include "server_directive.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Configuration {
private:
	std::vector<ServerDirective> servers_;

	std::vector<std::string> tokenize_file_content(std::ifstream& conf_file);
	std::vector<std::string> tokenize(std::string& line);
	bool isSpecialCharacter(const char& c);
	int parseConfiguration(std::vector<std::string>& tokens);

public:
	Configuration();
	~Configuration();
	Configuration(const Configuration& other);
	Configuration& operator=(const Configuration& other);

	int init(const std::string& path);
	const std::vector<ServerDirective>& getServerConfigurations() const;
};

std::ostream& operator<<(std::ostream& out, const Configuration& configuration);

#endif
