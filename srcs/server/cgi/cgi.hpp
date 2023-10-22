#ifndef CGI_HPP
#define CGI_HPP
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

class CGI {
private:
	std::map<std::string, std::string> meta_;
	std::string body;
};

#endif