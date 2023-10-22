#include "http_request.hpp"
#include "parse_sentense.hpp"
#include <cstdlib>

HttpRequest::HttpRequest()
	: status_(METHOD) {}

// Getter and Setter
void HttpRequest::setStatus(HTTP_REQUEST_STATUS const& status) {
	status_ = status;
}

void HttpRequest::setErrorStatus(HTTP_ERROR const& error_status) {
	error_status_ = error_status;
}
int HttpRequest::setMethod(std::string const& method) {
	if (method == "GET")
		method_ = GET;
	else if (method == "POST")
		method_ = POST;
	else if (method == "DELETE")
		method_ = DELETE;
	else {
		setStatus(ERROR);
		return (-1);
	}
	return (0);
}

int HttpRequest::setRequestPath(std::string const& request_path) {
	if (request_path == "\0") {
		setStatus(ERROR);
		return (-1);
	}
	request_path_ = request_path;
	return (0);
}

int HttpRequest::setVersion(std::string const& version) {
	if (version == "HTTP/1.0")
		version_ = HTTP_1_0;
	else if (version == "HTTP/1.1")
		version_ = HTTP_1_1;
	else {
		setStatus(ERROR);
		return (-1);
	}
	return (0);
}

// Parse
int HttpRequest::parseHttpRequest(std::string const& line) {
	switch (status_) {
		case METHOD:
			parseHttpMethod(line);
			break;
		case HEADER:
			parseHttpHeader(line);
			break;
		case BODY:
			parseHttpBody(line);
			break;
		default:
			setStatus(ERROR);
			break;
	}
	if (status_ == ERROR)
		return (-1);
	return (0);
}

int HttpRequest::parseHttpMethod(std::string const& line) {
	std::vector<std::string> method_vector;
	if (parseSentense(line, "%s %s %s", method_vector) == -1 || method_vector.size() != 3) {
		std::cerr << "Http Method Parse Error" << std::endl;
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	if (setMethod(method_vector[0]) < 0 || setRequestPath(method_vector[1]) < 0 ||
		setVersion(method_vector[2]) < 0) {
		std::cerr << "Http Method Parse Error" << std::endl;
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	setStatus(HEADER);
	return (0);
}

int HttpRequest::parseHttpHeader(std::string const& line) {
	std::vector<std::string> header_vector;
	if (line == "\0") {
		if (method_ == GET || getHeaderValue("Content-Length") == "0")
			setStatus(FINISHED);
		else
			setStatus(BODY);
		return (0);
	}
	// Later: http request compromise 0 space between : and value!
	else if (parseSentense(line, "%s: %s", header_vector) < 0 || header_vector.size() != 2) {
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	if (setHeaderValue(header_vector[0], header_vector[1]) < 0) {
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	return (0);
}

int HttpRequest::setHeaderValue(std::string const& key, std::string const& value) {
	header_[key] = value;
	return (0);
}

std::string HttpRequest::getHeaderValue(std::string const& key) {
	if (header_.count(key) == 0)
		return (std::string(""));
	else
		return (header_[key]);
}

int HttpRequest::parseHttpBody(std::string const& line) {
	// transfer-encoding
	if (getHeaderValue("Transfer-Encoding") == "chunked")
		return (parseChunkedBody(line));
	// content-length
	else if (getHeaderValue("Content-Length") != "")
		return (parseContentLengthBody(line));
	setStatus(ERROR);
	setErrorStatus(BAD_REQUEST);
	return (-1);
}

int HttpRequest::parseChunkedBody(std::string const& line) {
	if (chunked_status_ == CHUNKED_SIZE) {
		// no error handling
		// Caution ! strtol is C++ 11 function.
		chunked_size_ = std::strtol(line.c_str(), NULL, 16);
		chunked_status_ = CHUNKED_MESSAGE;
		if (chunked_size_ == 0)
			setStatus(FINISHED);
	} else if (line == "")
		chunked_status_ = CHUNKED_SIZE;
	else
		body_ += line;
	return (0);
}

int HttpRequest::parseContentLengthBody(std::string const& line) {
	// no error handling
	body_ += line;
	if (body_.size() == static_cast<size_t>(std::atoi(getHeaderValue("Content-Length").c_str())))
		setStatus(FINISHED);
	return (0);
}

void HttpRequest::getInfo(void) {
	std::cout << "method: " << method_ << std::endl;
	std::cout << "status: " << status_ << std::endl;
	std::cout << "version: " << version_ << std::endl;
	std::cout << "header" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = header_.begin(); iter != header_.end();
		 ++iter)
		std::cout << "first: " << iter->first << "second: " << iter->second << std::endl;
}
