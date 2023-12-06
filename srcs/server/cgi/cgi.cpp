#include "cgi.hpp"

namespace cgi {

Cgi::Cgi(server::HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: cgi_status_(UNDIFINED)
	, cgi_request_context_(cgi::CgiRequestContext(request, client_address, server_address))
	, pid_(-1)
	, status_(-1) {
	socket_vector_[0] = -1;
	socket_vector_[1] = -1;
}

Cgi::Cgi(const Cgi& other)
	: cgi_status_(other.cgi_status_)
	, cgi_request_context_(other.cgi_request_context_)
	, pid_(other.pid_)
	, status_(other.status_) {
	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

Cgi& Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		cgi_request_context_ = other.cgi_request_context_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		pid_ = other.pid_;
		status_ = other.status_;
	}
	return *this;
}

Cgi::~Cgi() {}

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

int Cgi::setupCgiRequestContext() {
	if (cgi_request_context_.setup() == -1) {
		return -1;
	}
	return 0;
}

int Cgi::setup() {
	if (setupInterProcessCommunication() == -1) {
		return -1;
	}
	if (setupCgiRequestContext() == -1) {
		return -1;
	}
	return 0;
}

int Cgi::executeCgi() {
	char** environ = cgi_request_context_.getCgiEnviron();
	char** argv = cgi_request_context_.getExecveArgv();
	server::http_method::HTTP_METHOD method = cgi_request_context_.getHttpRequest().getHttpMethod();

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
		if (method == server::http_method::POST) {
			if (dup2(socket_vector_[1], STDIN_FILENO) < 0) {
				std::cerr << "dup2() failed: " << strerror(errno) << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
		execve(argv[0], argv, environ);
		std::cerr << "execve() failed: " << strerror(errno) << std::endl;
		std::exit(EXIT_FAILURE);
	}
	if (method != server::http_method::POST) {
		close(socket_vector_[1]);
	}
	return 0;
}

int Cgi::readCgiOutput() {
	int sd = socket_vector_[0];
	char recv_buffer[BUFFER_SIZE];
	std::memset(recv_buffer, '\0', sizeof(recv_buffer));

	int recv_result = recv(sd, recv_buffer, sizeof(recv_buffer) - 1, 0);
	std::cout << "\033[30m"
			  << "  CGI output: " << recv_buffer << "\033[0m" << std::endl;
	if (recv_result > 0) {
		std::string recv_string(recv_buffer);
		cgi_output_ += recv_string;
	} else if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;

		// Todo : server error
		// ここでselectが∞るーぷするようになってしまう
		return -1;
	} else if (recv_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		waitpid(pid_, &status_, 0);
		if (WIFEXITED(status_)) {
			std::cout << "CGI process exited with status " << WEXITSTATUS(status_) << std::endl;
			setStatus(CGI_RECEVICEING_COMPLETE);
		} else if (WIFSIGNALED(status_)) {
			std::cout << "CGI process killed by signal " << WTERMSIG(status_) << std::endl;
		}
		return 0;
	}
	if (cgi_output_.find("\n\n") != std::string::npos &&
		cgi_output_.find("\n\n", cgi_output_.find("\n\n") + 1) != std::string::npos) {
		int signal;

		signal = SIGKILL;

		if (kill(pid_, signal) == -1) {
			// todo error
		}
		setStatus(CGI_RECEVICEING_COMPLETE);
	}
	return 0;
}

int Cgi::getSocketFd() const {
	return socket_vector_[0];
}

server::HttpRequest const& Cgi::getHttpRequest() const {
	return cgi_request_context_.getHttpRequest();
}

std::string const& Cgi::getCgiOutput() const {
	return cgi_output_;
}

void Cgi::setStatus(CGI_STATUS const status) {
	cgi_status_ = status;
}

CGI_STATUS Cgi::getStatus() const {
	return cgi_status_;
}

std::string const Cgi::sendResponse() {
	std::string tmp = cgi_output_.substr(0, BUFFER_SIZE - 1);
	cgi_output_ = cgi_output_.erase(0, BUFFER_SIZE - 1);
	if (cgi_output_.empty()) {
		setStatus(CGI_SENDING_COMPLETE);
	}
	return tmp;
}

std::ostream& operator<<(std::ostream& out, const Cgi& cgi) {

	out << "Cgi: " << std::endl;
	out << cgi.getCgiOutput() << std::endl;
	return out;
}

} // namespace server
