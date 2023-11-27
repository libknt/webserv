#ifndef CGI_HPP
#define CGI_HPP

// #include <sys/types.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>

namespace server {

class Cgi {
private:
	int socket_vector_[2];
	int pid_;
	int status_;
	int setNonBlocking(int sd);

public:
	Cgi();
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setupInterProcessCommunication();
};

} // namespace server

#endif // CGI_HPP