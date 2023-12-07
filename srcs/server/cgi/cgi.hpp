#ifndef CGI_HPP
#define CGI_HPP
#include "webserv.hpp"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <signal.h>

namespace cgi {
enum CGI_STATUS {
	UNDIFINED,
	EXECUTED,
};

class Cgi {
private:
	CGI_STATUS cgi_status_;
	std::map<std::string, std::string> meta_variables_;
	pid_t pid_;
	int socket_vector_[2];
	char** execve_argv_;
	char** environ_;
	int status_;

	int setNonBlocking(int sd);
	int setupInterProcessCommunication();
	char** mapToCharStarStar(std::map<std::string, std::string> const& map);
	char** createExecveArgv();

public:
	Cgi(const std::map<std::string, std::string> meta_variables);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setup();
	int execute();
	int getSocketFd(int const index) const;
	std::string findMetaVariable(std::string const& key) const;
	CGI_STATUS getStatus() const;
	pid_t getPid() const;
};

std::ostream& operator<<(std::ostream& out, const Cgi& cgi);

} // namespace

#endif // CGI_HPP