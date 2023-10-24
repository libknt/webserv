#ifndef CGI_HPP
#define CGI_HPP
#include "http_request.hpp"
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

namespace server {

class Cgi {
private:
	Cgi();
	std::map<std::string, std::string> meta_;
	std::string body_;
	HttpRequest request_;
	int create_meta_variables();

public:
	explicit Cgi(HttpRequest& request);
	~Cgi();
	int cgi_request();
	void getInfo();
};
}

#endif