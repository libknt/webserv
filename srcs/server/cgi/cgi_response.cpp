#include "cgi_response.hpp"

namespace cgi {

CgiResponse::CgiResponse()
	: stage_(NOT_STARTED)
	, pid_(-1)
	, status_(-1)
	, headers_stream_()
	, headers_()
	, body_()
	, content_length_(0) {
	socket_vector_[0] = -1;
	socket_vector_[1] = -1;
}

CgiResponse::CgiResponse(const CgiResponse& other)
	: stage_(other.stage_)
	, pid_(other.pid_)
	, status_(other.status_)
	, headers_stream_(other.headers_stream_)
	, headers_(other.headers_)
	, body_(other.body_)
	, content_length_(other.content_length_) {
	socket_vector_[0] = other.socket_vector_[0];
	socket_vector_[1] = other.socket_vector_[1];
}

CgiResponse& CgiResponse::operator=(const CgiResponse& other) {
	if (this != &other) {
		stage_ = other.stage_;
		pid_ = other.pid_;
		status_ = other.status_;
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		headers_stream_ = other.headers_stream_;
		headers_ = other.headers_;
		body_ = other.body_;
		content_length_ = other.content_length_;
	}
	return *this;
}

CgiResponse::~CgiResponse() {}

void CgiResponse::setStage(CGI_RESPONSE_STAGE const stage) {
	stage_ = stage;
}

void CgiResponse::setPid(pid_t pid) {
	pid_ = pid;
}
void CgiResponse::setSocketFd(int index, int fd) {
	socket_vector_[index] = fd;
}

CGI_RESPONSE_STAGE CgiResponse::getStage() const {
	return stage_;
}

static std::string ltrim(const std::string& s) {
	size_t start = s.find_first_not_of(" \t");
	return (start == std::string::npos) ? "" : s.substr(start);
}

static std::string rtrim(const std::string& s) {
	size_t end = s.find_last_not_of(" \t");
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

static std::string trim(const std::string& s) {
	return rtrim(ltrim(s));
}

static std::string toLower(const std::string& s) {
	std::string result;
	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it) {
		result += static_cast<char>(std::tolower(*it));
	}
	return result;
}

void CgiResponse::parseHeaders() {
	std::istringstream stream(headers_stream_);
	std::string line;

	headers_.clear();

	while (std::getline(stream, line, '\n')) {
		std::string::size_type pos = line.find("\r\n");
		if (pos != std::string::npos) {
			line.erase(pos);
		}
		pos = line.find('\n');
		if (pos != std::string::npos) {
			line.erase(pos);
		}
		if (line.empty()) {
			break;
		}

		pos = line.find(':');
		if (pos != std::string::npos) {
			std::string key = toLower(trim(line.substr(0, pos)));
			std::string value = trim(line.substr(pos + 1));
			if (!value.empty()) {
				headers_.insert(std::pair<std::string, std::string>(key, value));
			}
		}
	}
	headers_stream_.clear();
}

void CgiResponse::advanceResponseProcessing(std::string const& value) {
	std::string output(value);

	if (stage_ == NOT_STARTED) {
		return;
	}
	if (stage_ == HEADERS_SENT) {
		if (!headers_stream_.empty() && headers_stream_[headers_stream_.size() - 1] == '\n' &&
			output[0] == '\n') {
			stage_ = BODY_SENT;
		} else {
			std::string::size_type pos = output.find("\n\n");
			if (pos != std::string::npos) {
				stage_ = BODY_SENT;
				headers_stream_ += output.substr(0, pos + 1);
				output.erase(0, pos + 2);
				parseHeaders();
			} else {
				headers_stream_ += output;
			}
		}
	}
	if (stage_ == BODY_SENT) {
		if (!body_.empty() && body_[body_.size() - 1] == '\n' && output[0] == '\n') {
			body_ += output;
			stage_ = COMPLETE;
			return;
		} else {
			std::string::size_type pos = output.find("\n\n");
			if (pos != std::string::npos) {
				stage_ = COMPLETE;
				body_ += output;
				content_length_ = body_.size();
			}
			body_ += output;
		}
	}
}

int CgiResponse::readCgiReponse() {
	if (stage_ == NOT_STARTED) {
		std::cout << "cgi_response: NOT_STARTED" << std::endl;
		return -1;
	}
	int sd = socket_vector_[0];
	char buffer[BUFFER_SIZE];
	std::memset(buffer, '\0', sizeof(buffer));

	int recv_result = recv(sd, buffer, BUFFER_SIZE - 1, 0);
	if (recv_result > 0) {
		advanceResponseProcessing(std::string(buffer));
	} else if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		return -1;
	} else if (recv_result == 0) {
		std::cout << "  Connection closed" << std::endl;
		if (waitpid(pid_, &status_, 0) > 0) {
			if (WIFEXITED(status_)) {
				std::cout << "\e[33m"
						  << "CGI process exited with status " << WEXITSTATUS(status_) << "\e[m"
						  << std::endl;
				if (WEXITSTATUS(status_) != 0) {
					return -1;
				}
			} else if (WIFSIGNALED(status_)) {
				std::cout << "\e[33m"
						  << "CGI process killed by signal " << WTERMSIG(status_) << "\e[m"
						  << std::endl;
			}
			stage_ = COMPLETE;
		} else {
			std::cerr << "waitpid failed" << std::endl;
			return -1;
		}
	}
	return 0;
}

int CgiResponse::getSocketFd(int index) const {
	return socket_vector_[index];
}

pid_t CgiResponse::getPid() const {
	return pid_;
}

int CgiResponse::getStatus() const {
	return status_;
}

const std::map<std::string, std::string>& CgiResponse::getHeaders() const {
	return headers_;
}

const std::string& CgiResponse::getBody() const {
	return body_;
}

std::string const CgiResponse::getHeaderValue(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = headers_.find(key);
	if (it == headers_.end()) {
		return "";
	}
	return it->second;
}

std::size_t CgiResponse::getContentLength() const {
	return content_length_;
}

std::ostream& operator<<(std::ostream& out, const CgiResponse& cgi_response) {
	out << "CgiResponse: " << std::endl;
	out << "  stage: " << cgi_response.getStage() << std::endl;
	out << "  pid: " << cgi_response.getPid() << std::endl;
	out << "  socket_vector_[0]: " << cgi_response.getSocketFd(0) << std::endl;
	out << "  socket_vector_[1]: " << cgi_response.getSocketFd(1) << std::endl;
	out << "  headers: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = cgi_response.getHeaders().begin();
		 it != cgi_response.getHeaders().end();
		 ++it) {
		out << "    " << it->first << ": " << it->second << std::endl;
	}
	out << "  body: " << std::endl;
	out << cgi_response.getBody() << std::endl;
	return out;
}

} // namespace cgi
