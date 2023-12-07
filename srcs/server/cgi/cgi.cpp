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
	if (execve_argv_) {
		for (int i = 0; execve_argv_[i]; ++i) {
			delete[] execve_argv_[i];
		}
		delete[] execve_argv_;
	}
	if (environ_) {
		for (int i = 0; environ_[i]; ++i) {
			delete[] environ_[i];
		}
		delete[] environ_;
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

// シバン（shebang）
// #!/usr/local/bin/python3
int Cgi::shebang(std::string file, std::string& path) {
	std::ifstream infile(file.c_str());
	if (!infile) {
		std::cout << "file open error: " << file << std::endl;
		return -1;
	}
	std::string line;
	std::getline(infile, line);
	path.clear();
	if (line.empty()) {
		return -1;
	} else if (line.size() > 2 && line[0] == '#' && line[1] == '!') {
		line = line.substr(2);
		// int access_flag = access(line.c_str(), X_OK);
		// if (access_flag == 0) {
		// 	std::cout << "access() success: " << line << std::endl;
		path = line;
		// } else {
		// 	std::cout << "access() failed: " << line << std::endl;
		// }
	}
	return 0;
}

char** Cgi::createExecveArgv() {
	std::vector<std::string> argv_vector;
	std::string path;
	std::string script = findMetaVariable("PATH_TRANSLATED") + findMetaVariable("SCRIPT_NAME");
	shebang(script, path);
	if (!path.empty()) {
		std::istringstream iss(path);
		std::string token;
		while (std::getline(iss, token, ' ')) {
			if (!token.empty()) {
				argv_vector.push_back(token);
			}
		}
	} else {
		argv_vector.push_back("/usr/bin/python3");
	}
	argv_vector.push_back(script);

	char** argv = new (std::nothrow) char*[argv_vector.size() + 1];
	if (!argv) {
		return NULL;
	}

	for (size_t i = 0; i < argv_vector.size(); ++i) {
		argv[i] = new (std::nothrow) char[argv_vector[i].size() + 1];
		if (!argv[i]) {
			for (size_t j = 0; j < i; ++j) {
				delete[] argv[j];
			}
			delete[] argv;
			return NULL;
		}
		std::strcpy(argv[i], argv_vector[i].c_str());
	}
	argv[argv_vector.size()] = NULL;
	argv_vector.clear();
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
