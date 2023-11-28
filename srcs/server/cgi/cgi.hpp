#ifndef CGI_HPP
#define CGI_HPP
#include "cgi_request_context.hpp"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include "webserv.hpp"

namespace server {

class Cgi {
private:
	CgiRequestContext cgi_request_context_;
	int socket_vector_[2];
	int pid_;
	int status_;
	std::string cgi_output_;
	int setNonBlocking(int sd);
	int setupCgiRequestContext();
	int setupInterProcessCommunication();

public:
	Cgi(HttpRequest const& request,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setup();
	int executeCgi();
	int readCgiOutput();
	int getSocketFd() const;	
	std::string const& getCgiOutput() const;
};

std::ostream& operator<<(std::ostream& out, const Cgi& cgi);

} // namespace server

#endif // CGI_HPP