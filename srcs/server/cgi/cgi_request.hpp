#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP
#include "webserv.hpp"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <signal.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace cgi {
enum CGI_STATUS {
	UNDIFINED,
	EXECUTED,
	ERROR,
};

class CgiRequest {
private:
	CGI_STATUS cgi_status_;
	std::map<std::string, std::string> meta_variables_;
	pid_t pid_;
	int socket_vector_[2];
	char** execve_argv_;
	char** environ_;
	int status_;
	std::string body_;

	int setupInterProcessCommunication();
	int setNonBlocking(int sd);
	char** createExecveArgv();
	int shebang(std::string file, std::string& path);
	char** mapToCharPtrPtr(std::map<std::string, std::string> const& map);

public:
	CgiRequest();
	CgiRequest(const CgiRequest& other);
	CgiRequest& operator=(const CgiRequest& other);
	~CgiRequest();
	int setup();
	int execute();
	std::string extractBodySegment(std::size_t max_size);
	CGI_STATUS getStatus() const;
	std::map<std::string, std::string> getMetaVariables() const;
	void setMetaVariable(std::map<std::string, std::string> const& meta_variables);
	std::string findMetaVariable(std::string const& key) const;
	pid_t getPid() const;
	int getSocketFd(int const index) const;
	char const* const* getExecveArgv() const;
	char const* const* getEnviron() const;
	const std::string& getBody() const;
	void setBody(std::string const& body);
};

std::ostream& operator<<(std::ostream& out, const CgiRequest& cgi);

} // namespace

#endif // CGI_HPP