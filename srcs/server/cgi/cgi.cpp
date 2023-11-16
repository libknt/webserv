#include "cgi.hpp"

namespace server {

Cgi::~Cgi() {
	if (execve_argv != NULL) {
		for (int i = 0; execve_argv[i] != NULL; i++) {
			delete[] execve_argv[i];
		}
		delete[] execve_argv;
	}
	close(socket_vector_[0]);
	close(socket_vector_[1]);
}

Cgi::Cgi(const Cgi& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_)
	, has_body(false) {

	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];

	execve_argv = DeepCopyCharPointerArray(other.execve_argv);
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		meta_variables_ = other.meta_variables_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		execve_argv = DeepCopyCharPointerArray(other.execve_argv);
		has_body = other.has_body;
	}
	return *this;
}

Cgi::Cgi(const HttpRequest& request)
	: request_(request)
	, meta_variables_(request)
	, execve_argv(NULL)
	, has_body(false) {
	socket_vector_[0] = (-1);
	socket_vector_[1] = (-1);
}

int Cgi::setupInterProcessCommunication() {
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vector_) == -1) {
		std::cerr << "socketpair() failed(): " << strerror(errno) << std::endl;
		return -1;
	}
	for(int i = 0; i < 2; i++) {
		int flags = fcntl(socket_vector_[i], F_GETFL, 0);
		if( flags < 0) {
			std::cerr << "fcntl() failed(): " << strerror(errno) << std::endl;
			return -1;
		}

		flags |= O_NONBLOCK;
		flags = fcntl(socket_vector_[i], F_SETFL, flags);
		if(flags < 0) {
			std::cerr << "fcntl() failed(): " << strerror(errno) << std::endl;
			return -1;
		}
	}
	if (hasBody()) {
		has_body = true;
	}
	return 0;
}

int Cgi::setupExecveArgv() {
	execve_argv = new (std::nothrow) char*[3];
	if (!execve_argv) {
		return -1;
	}

	std::string path = "/usr/bin/python3";

	execve_argv[0] = new (std::nothrow) char[path.size() + 1];
	if (!execve_argv[0]) {
		delete[] execve_argv;
		return -1;
	}
	std::strcpy(execve_argv[0], path.c_str());

	std::string script = meta_variables_.getMetaVariables("SCRIPT_NAME");
	script = "/home/ubuntu2204/Documents/prg/42tokyo/webserv" + script;
	execve_argv[1] = new (std::nothrow) char[script.size() + 1];
	if (!execve_argv[1]) {
		delete[] execve_argv[0];
		delete[] execve_argv;
		return -1;
	}
	std::strcpy(execve_argv[1], script.c_str());

	execve_argv[2] = NULL;

	return 0;
}

int Cgi::setup() {
	if (meta_variables_.setup() < 0) {
		std::cerr << "Cgi::setup(): meta_variables_.setup() failed" << std::endl;
		return -1;
	}
	if (meta_variables_.createCgiEnviron() < 0) {
		std::cerr << "Cgi::createCgiEnviron(): meta_variables_.createCgiEnviron() failed"
				  << std::endl;
		return -1;
	}
	if (setupInterProcessCommunication() < 0) {
		std::cerr << "Cgi::setupInterProcessCommunication() failed" << std::endl;
		return -1;
	}
	if (setupExecveArgv() < 0) {
		std::cerr << "Cgi::setupExecveArgv() failed" << std::endl;
		return -1;
	}
	return 0;
}

int Cgi::executeCgi() {
	char** env = meta_variables_.getCgiEnviron();
	pid = fork();
	if (pid == 0) {
		close(socket_vector_[0]);
		if (dup2(socket_vector_[1], STDOUT_FILENO) < 0) {
			std::exit(EXIT_FAILURE);
		}
		if (has_body) {
			if (dup2(socket_vector_[1], STDIN_FILENO) < 0) {
				std::exit(EXIT_FAILURE);
			}
		}
		char** exec_argv = getExecveArgv();
		execve("/usr/bin/python3", exec_argv, env);
		std::exit(EXIT_FAILURE);
	}
	close(socket_vector_[1]);
	// FD_SET(socket_vector_[0], &readfds);
	return 0;
}

int Cgi::createCgiResponse() {
	int status;
	char buf[BUFFER_SIZE];
	int nbytes;
	nbytes = read(socket_vector_[0], buf, BUFFER_SIZE - 1);
	buf[nbytes] = '\0';
	std::cout << "buf: " << buf << std::endl;
	if (nbytes == 0) {
		close(socket_vector_[0]);
		waitpid(pid, &status, 0);
		return 1;
	}
	return 0;
}

bool Cgi::hasBody() const {
	return !request_.getBody().empty();
}

bool Cgi::getHasbody() const {
	return has_body;
}

char** Cgi::getExecveArgv() const {
	return execve_argv;
}

int Cgi::getSocketVector(int index) const {
	return socket_vector_[index];
}

}