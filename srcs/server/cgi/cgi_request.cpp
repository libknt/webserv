#include "cgi_request.hpp"

namespace cgi {

static char** DeepCopy(char** src) {
	if (!src)
		return NULL;

	int count = 0;
	while (src[count])
		count++;

	char** dst = new char*[count + 1];
	if (!dst)
		return NULL;

	dst[count] = NULL;

	for (int i = 0; i < count; ++i) {
		int len = std::strlen(src[i]);
		dst[i] = new char[len + 1];
		if (!dst[i]) {
			while (--i >= 0)
				delete[] dst[i];
			delete[] dst;
			return NULL;
		}
		std::strcpy(dst[i], src[i]);
	}
	return dst;
}

CgiRequest::CgiRequest()
	: cgi_status_(UNDIFINED)
	, meta_variables_()
	, pid_(-1)
	, execve_argv_(NULL)
	, environ_(NULL)
	, status_(-1) {
	socket_vector_[0] = -1;
	socket_vector_[1] = -1;
}

CgiRequest::CgiRequest(const CgiRequest& other)
	: cgi_status_(other.cgi_status_)
	, meta_variables_(other.meta_variables_)
	, pid_(other.pid_)
	, status_(other.status_) {
	execve_argv_ = DeepCopy(other.execve_argv_);
	environ_ = DeepCopy(other.environ_);
	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

CgiRequest& CgiRequest::operator=(const CgiRequest& other) {
	if (this != &other) {
		char** new_argv = DeepCopy(other.execve_argv_);
		char** new_env = DeepCopy(other.environ_);

		if (execve_argv_ != NULL) {
			for (int i = 0; execve_argv_[i]; ++i) {
				delete[] execve_argv_[i];
			}
			delete[] execve_argv_;
		}
		if (environ_ != NULL) {
			for (int i = 0; environ_[i]; ++i) {
				delete[] environ_[i];
			}
			delete[] environ_;
		}

		execve_argv_ = new_argv;
		environ_ = new_env;
		cgi_status_ = other.cgi_status_;
		meta_variables_ = other.meta_variables_;
		pid_ = other.pid_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		status_ = other.status_;
	}
	return *this;
}

CgiRequest::~CgiRequest() {
	if (pid_ != -1) {
		kill(pid_, SIGKILL);
	}

	if (execve_argv_ != NULL) {
		std::cout << execve_argv_[0] << std::endl;
	}
	if (execve_argv_ != NULL) {
		for (int i = 0; execve_argv_[i]; ++i) {
			delete[] execve_argv_[i];
		}
		delete[] execve_argv_;
		execve_argv_ = NULL;
	}
	if (environ_ != NULL) {
		for (int i = 0; environ_[i]; ++i) {
			delete[] environ_[i];
		}
		delete[] environ_;
	}
	environ_ = NULL;
	close(socket_vector_[0]);
	close(socket_vector_[1]);
}

int CgiRequest::setup() {
	if (setupInterProcessCommunication() == -1) {
		return -1;
	}
	execve_argv_ = createExecveArgv();
	if (!execve_argv_) {
		return -1;
	}
	environ_ = mapToCharPtrPtr(meta_variables_);
	if (!environ_) {
		return -1;
	}
	return 0;
}

int CgiRequest::execute() {
	char** argv = execve_argv_;
	char** environ = environ_;
	std::string const method = findMetaVariable("REQUEST_METHOD");

	std::cerr << "execute: " << argv[0] << " : " << argv[1] << std::endl;
	pid_ = fork();
	if (pid_ == -1) {
		std::cerr << "fork() failed: " << strerror(errno) << std::endl;
		return -1;
	}
	if (pid_ == 0) {
		std::string directory = argv[1];
		size_t last_slash_pos = directory.find_last_of('/');
		if (last_slash_pos != std::string::npos) {
			directory = directory.substr(0, last_slash_pos);
			if (chdir(directory.c_str()) != 0) {
				std::cerr << "chdir() failed: " << strerror(errno) << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
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
	start_time_ = time(0);
	return 0;
}

std::string CgiRequest::extractBodySegment(std::size_t max_size) {
	if (body_.empty()) {
		return "";
	}
	std::string temp = body_.substr(0, max_size);
	body_ = body_.erase(0, max_size);
	return temp;
}

int CgiRequest::setupInterProcessCommunication() {
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

int CgiRequest::setNonBlocking(int sd) {
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

char** CgiRequest::createExecveArgv() {
	std::vector<std::string> argv_vector;
	std::string path;
	std::string script = findMetaVariable("PATH_TRANSLATED") + findMetaVariable("SCRIPT_NAME");
	shebang(script, path);
	if (path.empty()) {
		return NULL;
	}
	std::istringstream iss(path);
	std::string token;
	while (std::getline(iss, token, ' ')) {
		if (!token.empty()) {
			argv_vector.push_back(token);
		}
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

// シバン（shebang）
// #!/usr/local/bin/python3
int CgiRequest::shebang(std::string file, std::string& path) {
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
		path = line;
	}
	return 0;
}

char** CgiRequest::mapToCharPtrPtr(const std::map<std::string, std::string>& map) {
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

CGI_STATUS CgiRequest::getStatus() const {
	return cgi_status_;
}

std::map<std::string, std::string> CgiRequest::getMetaVariables() const {
	return meta_variables_;
}

void CgiRequest::setMetaVariable(std::map<std::string, std::string> const& meta_variables) {
	meta_variables_ = meta_variables;
}

std::string CgiRequest::findMetaVariable(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = meta_variables_.find(key);
	if (it == meta_variables_.end()) {
		return "";
	}
	return it->second;
}

pid_t CgiRequest::getPid() const {
	return pid_;
}
void CgiRequest::setPid(pid_t pid) {
	pid_ = pid;
}

int CgiRequest::getSocketFd(int const index) const {
	return socket_vector_[index];
}

char const* const* CgiRequest::getExecveArgv() const {
	return execve_argv_;
}

char const* const* CgiRequest::getEnviron() const {
	return environ_;
}

const std::string& CgiRequest::getBody() const {
	return body_;
}

void CgiRequest::setBody(std::string const& body) {
	body_ = body;
}

void CgiRequest::checkTimeout() {
	if (pid_ != -1) {
		time_t current_time = time(0);
		double seconds_passed = difftime(current_time, start_time_);
		if (seconds_passed >= 10.0) {
			std::cout << "CGI Timeout" << std::endl;
			std::cout << pid_ << std::endl;
			kill(pid_, SIGKILL);
			pid_ = -1;
		}
	}
}

std::ostream& operator<<(std::ostream& out, const CgiRequest& cgi) {
	// out << "\e[1;32m";
	out << "CgiRequest: " << std::endl;
	out << "cgi_status_: " << cgi.getStatus() << std::endl;
	out << "pid_: " << cgi.getPid() << std::endl;
	out << "socket_vector_[0]: " << cgi.getSocketFd(0) << std::endl;
	out << "socket_vector_[1]: " << cgi.getSocketFd(1) << std::endl;
	out << "execve_argv_: " << std::endl;
	char const* const* execve_argv = cgi.getExecveArgv();
	for (int i = 0; execve_argv[i]; ++i) {
		out << "  " << execve_argv[i] << std::endl;
	}
	out << "environ_: " << std::endl;
	char const* const* environ = cgi.getEnviron();
	for (int i = 0; environ[i]; ++i) {
		out << "  " << environ[i] << std::endl;
	}
	out << "body_: " << cgi.getBody() << std::endl;
	// out << "\e[m";
	return out;
}

} // namespace server
