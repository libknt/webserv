#ifndef CGI_HPP
#define CGI_HPP

#include "cgi_meta_variables.hpp"
#include "define.hpp"
#include "http_request.hpp"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

namespace server {

class Cgi {
private:
	Cgi();
	int setupInterProcessCommunication();
	int setupExecveArgv();

	const HttpRequest& request_;
	CgiMetaVariables meta_variables_;
	int socket_vector_[2];
	char** execve_argv;
	bool has_body;
	pid_t pid;
	std::string response_body;

public:
	Cgi(const HttpRequest& request);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setup();
	bool hasBody() const;
	bool getHasbody() const;
	char** getExecveArgv() const;
	int executeCgi();
	int createCgiResponse();
	int getSocketVector(int index) const;
};

} // namespace cgi

#endif