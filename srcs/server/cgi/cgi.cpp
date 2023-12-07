#include "cgi.hpp"

namespace cgi {

Cgi::Cgi(const std::map<std::string, std::string> meta_variables)
	: cgi_status_(UNDIFINED)
	, meta_variables_(meta_variables)
	, pid_(-1)
	, execve_argv_(NULL)
	, environ_(NULL)
	, status_(-1) {
	socket_vector_[0] = -1;
	socket_vector_[1] = -1;
}

Cgi::Cgi(const Cgi& other)
	: cgi_status_(other.cgi_status_)
	, meta_variables_(other.meta_variables_)
	, pid_(other.pid_)
	, status_(other.status_) {
	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		cgi_status_ = other.cgi_status_;
		meta_variables_ = other.meta_variables_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		pid_ = other.pid_;
		status_ = other.status_;
	}
	return *this;
}

Cgi::~Cgi() {
	if (pid_ != -1) {
		kill(pid_, SIGKILL);
	}
	close(socket_vector_[0]);
	close(socket_vector_[1]);
}

int Cgi::setNonBlocking(int sd) {
	int flags = fcntl(sd, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "fcntl() get flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	flags |= O_NONBLOCK;
	flags = fcntl(sd, F_SETFL, flags);
	if (flags < 0) {
		std::cerr << "fcntl() set flags failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 0;
}

int Cgi::setupInterProcessCommunication() {
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vector_) == -1) {
		std::cerr << "socketpair() failed(): " << strerror(errno) << std::endl;
		return -1;
	}
	if (setNonBlocking(socket_vector_[0]) == -1) {
		return -1;
	}
	if (setNonBlocking(socket_vector_[1]) == -1) {
		return -1;
	}
	return 0;
}

char** Cgi::mapToCharStarStar(const std::map<std::string, std::string>& map) {
	typedef std::map<std::string, std::string>::const_iterator MapIterator;
	char** char_star_star = new (std::nothrow) char*[map.size() + 1];
	if (!char_star_star) {
		std::cerr << "Memory allocation failed" << std::endl;
		return NULL;
	}

	int i = 0;
	for (MapIterator it = map.begin(); it != map.end(); ++it) {
		std::string temp = it->first + "=" + it->second;
		char_star_star[i] = new (std::nothrow) char[temp.size() + 1];
		if (!char_star_star[i]) {
			std::cerr << "Memory allocation failed" << std::endl;
			for (int j = 0; j < i; ++j) {
				delete[] char_star_star[j];
			}
			delete[] char_star_star;
			return NULL;
		}
		std::strcpy(char_star_star[i], temp.c_str());
		++i;
	}
	char_star_star[i] = NULL;
	return char_star_star;
}

char** Cgi::createExecveArgv() {
	char** argv = new (std::nothrow) char*[3];
	if (!argv) {
		return NULL;
	}

	std::string path = "/usr/bin/python3";

	argv[0] = new (std::nothrow) char[path.size() + 1];
	if (!argv[0]) {
		delete[] argv;
		return NULL;
	}
	std::strcpy(argv[0], path.c_str());

	std::string script = findMetaVariable("SCRIPT_NAME");
	script = "/home/ubuntu2204/Documents/prg/42tokyo/webserv" + script;
	argv[1] = new (std::nothrow) char[script.size() + 1];
	if (!argv[1]) {
		delete[] argv[0];
		delete[] argv;
		return NULL;
	}
	std::strcpy(argv[1], script.c_str());

	argv[2] = NULL;
	return argv;
}

int Cgi::setup() {
	if (setupInterProcessCommunication() == -1) {
		return -1;
	}
	execve_argv_ = createExecveArgv();
	if (!execve_argv_) {
		return -1;
	}
	environ_ = mapToCharStarStar(meta_variables_);
	if (!environ_) {
		return -1;
	}
	return 0;
}

int Cgi::execute() {
	char** argv = execve_argv_;
	char** environ = environ_;
	std::string const method = findMetaVariable("REQUEST_METHOD");

	pid_ = fork();
	if (pid_ == -1) {
		std::cerr << "fork() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	if (pid_ == 0) {
		close(socket_vector_[0]);
		if (dup2(socket_vector_[1], STDOUT_FILENO) < 0) {
			std::cerr << "dup2() failed: " << strerror(errno) << std::endl;
			std::exit(EXIT_FAILURE);
		}
		if (method == "POST") {
			if (dup2(socket_vector_[1], STDIN_FILENO) < 0) {
				std::cerr << "dup2() failed: " << strerror(errno) << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
		// todo fcntl FD_CLOEXEC
		execve(argv[0], argv, environ);
		std::cerr << "execve() failed: " << strerror(errno) << std::endl;
		std::exit(EXIT_FAILURE);
	}
	close(socket_vector_[1]);
	std::cout << "pid: " << pid_ << std::endl;
	cgi_status_ = EXECUTED;
	return 0;
}

std::string Cgi::findMetaVariable(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = meta_variables_.find(key);
	if (it == meta_variables_.end()) {
		return "";
	}
	return it->second;
}

int Cgi::getSocketFd(int const index) const {
	return socket_vector_[index];
}

CGI_STATUS Cgi::getStatus() const {
	return cgi_status_;
}

pid_t Cgi::getPid() const {
	return pid_;
}

std::ostream& operator<<(std::ostream& out, const Cgi& cgi) {

	out << "Cgi: " << std::endl;
	out << cgi.getStatus() << std::endl;
	return out;
}

} // namespace server
