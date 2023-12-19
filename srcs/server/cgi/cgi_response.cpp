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
		socket_vector_[0] = other.socket_vector_[0];
		socket_vector_[1] = other.socket_vector_[1];
		pid_ = other.pid_;
		status_ = other.status_;
		headers_stream_ = other.headers_stream_;
		headers_ = other.headers_;
		body_ = other.body_;
		content_length_ = other.content_length_;
	}
	return *this;
}

CgiResponse::~CgiResponse() {}

int CgiResponse::readCgiResponse() {
	if (stage_ == NOT_STARTED) {
		std::cerr << "cgi_response: NOT_STARTED" << std::endl;
		return -1;
	}

	int sd = socket_vector_[0];
	char buffer[BUFFER_SIZE];
	std::memset(buffer, '\0', sizeof(buffer));

	int recv_result = recv(sd, buffer, BUFFER_SIZE - 1, 0);
	if (recv_result > 0) {
		return advanceResponseProcessing(std::string(buffer));
	} else {
		return handleRecvError(recv_result);
	}

	return 0;
}

CGI_RESPONSE_STAGE CgiResponse::getStage() const {
	return stage_;
}

void CgiResponse::setStage(CGI_RESPONSE_STAGE const stage) {
	stage_ = stage;
}

int CgiResponse::getSocketFd(int index) const {
	return socket_vector_[index];
}

void CgiResponse::setSocketFd(int index, int fd) {
	socket_vector_[index] = fd;
}

pid_t CgiResponse::getPid() const {
	return pid_;
}

void CgiResponse::setPid(pid_t pid) {
	pid_ = pid;
}

int CgiResponse::getStatus() const {
	return status_;
}

const std::map<std::string, std::string>& CgiResponse::getHeaders() const {
	return headers_;
}

bool CgiResponse::isHeaderKeyPresent(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = headers_.find(key);
	if (it == headers_.end()) {
		return false;
	}
	return true;
}

std::string const CgiResponse::getHeaderValue(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = headers_.find(key);
	if (it == headers_.end()) {
		return "";
	}
	return it->second;
}

const std::string& CgiResponse::getBody() const {
	return body_;
}

std::size_t CgiResponse::getContentLength() const {
	return content_length_;
}

// todo これcgi側がすべきことではない
int CgiResponse::advanceResponseProcessing(std::string const& value) {
	std::string output(value);

	if (stage_ == NOT_STARTED) {
		return -1;
	}
	if (stage_ == HEADERS_SENT) {
		if (processHeaders(output) < 0) {
			return -1;
		}
	}
	if (stage_ == BODY_SENT) {
		processBody(output);
	}
	return 0;
}

int CgiResponse::processHeaders(std::string& output) {
	if (!headers_stream_.empty() && headers_stream_[headers_stream_.size() - 1] == '\n' &&
		output[0] == '\n') {
		stage_ = BODY_SENT;
		output = output.erase(0, 1);
		return 0;
	}

	const std::string headerDelimiter = "\n\n";
	std::string::size_type pos = output.find(headerDelimiter);
	if (pos != std::string::npos) {
		stage_ = BODY_SENT;
		headers_stream_ += output.substr(0, pos + 1);
		output.erase(0, pos + headerDelimiter.length());
		if (parseHeaders() < 0) {
			return -1;
		}
	} else {
		headers_stream_ += output;
	}
	return 0;
}

int CgiResponse::parseHeaders() {
	std::istringstream stream(headers_stream_);
	headers_.clear();
	std::string line;
	while (std::getline(stream, line, '\n')) {
		if (line.empty()) {
			break;
		}
		if (line[line.size() - 1] == '\n') {
			line.erase(line.size() - 1);
		}
		if (parseHeaderLine(line) < 0) {
			return -1;
		}
	}
	headers_stream_.clear();
	return 0;
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

int CgiResponse::parseHeaderLine(const std::string& line) {
	size_t pos = line.find(':');
	if (pos != std::string::npos) {
		std::string key = toLower(trim(line.substr(0, pos)));
		std::string value = trim(line.substr(pos + 1));
		// if (!value.empty()) {
		if (headers_.find(key) != headers_.end()) {
			return -1;
		}
		headers_.insert(std::make_pair(key, value));
		// }
	}
	return 0;
}

void CgiResponse::processBody(std::string& output) {
	if (!body_.empty() && body_[body_.size() - 1] == '\n' && output[0] == '\n') {
		body_ += output;
		stage_ = COMPLETE;
		return;
	}
	const std::string bodyDelimiter = "\n\n";
	std::string::size_type pos = output.find(bodyDelimiter);
	if (pos != std::string::npos) {
		stage_ = COMPLETE;
		body_ += output;
		content_length_ = body_.size();
	} else {
		body_ += output;
	}
}

int CgiResponse::handleRecvError(int recv_result) {
	if (recv_result < 0) {
		std::cerr << "recv() failed: " << strerror(errno) << std::endl;
		return -1;
	}

	std::cout << "  Connection closed" << std::endl;
	if (processChildExit()) {
		stage_ = COMPLETE;
	} else {
		return -1;
	}

	return 0;
}

bool CgiResponse::processChildExit() {
	if (waitpid(pid_, &status_, 0) <= 0) {
		std::cerr << "waitpid failed" << std::endl;
		return false;
	}

	if (WIFEXITED(status_)) {
		std::cout << "\e[33m"
				  << "CGI process exited with status " << WEXITSTATUS(status_) << "\e[m"
				  << std::endl;
		return WEXITSTATUS(status_) == 0;
	}

	if (WIFSIGNALED(status_)) {
		std::cout << "\e[33m"
				  << "CGI process killed by signal " << WTERMSIG(status_) << "\e[m" << std::endl;
	}

	return true;
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
