#ifndef CGI_HPP
#define CGI_HPP
#include "cgi_meta_variables.hpp"
#include "http_request.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace server {

class Cgi {
private:
	Cgi();
	CgiMetaVariables meta_;
	std::string body_;
	HttpRequest request_;
	std::string path_;
	std::string script_;
	std::string getFileExtension(const std::string& filename);
	int extension(std::string filename, std::string& path);
	int shebang(std::string file, std::string& path);
	int execCgi();
	int createExecArgv();

public:
	explicit Cgi(HttpRequest& request);
	~Cgi();
	int cgiRequest();
	void getInfo();
};
}

#endif