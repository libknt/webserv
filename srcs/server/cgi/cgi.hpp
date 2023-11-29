#ifndef CGI_HPP
#define CGI_HPP
#include "cgi_request_context.hpp"
#include "webserv.hpp"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

namespace server {

namespace cgi_status {

enum CGI_STATUS {
	BODY_SENDING,
	CGI_RECEVICEING_COMPLETE,
	CGI_SENDING_COMPLETE,
	ERROR_OCCURRED,
	UNDIFINED,
};

} // namespace cgi_status

class Cgi {
private:
	cgi_status::CGI_STATUS cgi_status_;
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
	HttpRequest const& getHttpRequest() const;
	std::string const& getCgiOutput() const;
	void setStatus(cgi_status::CGI_STATUS const status);
	cgi_status::CGI_STATUS getStatus() const;
	std::string const sendResponse();
};

std::ostream& operator<<(std::ostream& out, const Cgi& cgi);

} // namespace server

#endif // CGI_HPP