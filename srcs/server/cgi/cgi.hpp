#ifndef CGI_HPP
#define CGI_HPP
#include "cgi_meta_variables.hpp"
#include "http_request.hpp"
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>
#include <fstream>

namespace server {

class Cgi {
private:
	Cgi();
	CgiMetaVariables meta_;
	std::string body_;
	HttpRequest request_;
	std::string getFileExtension(const std::string& filename);
int extension(std::string filename, std::string& path);
int shebang(std::string file, std::string& path);
	int exec_cgi();

public:
	explicit Cgi(HttpRequest& request);
	~Cgi();
	int cgi_request();
	void getInfo();
};
}

#endif