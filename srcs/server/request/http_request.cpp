#include "http_request.hpp"
#include "parse_sentense.hpp"
#include <cstdlib>

namespace server {
HttpRequest::HttpRequest()
	: status_(http_request_status::METHOD)
	, method_(http_method::UNDEFINED)
	, version_(http_version::UNDEFINED)
	, error_status_(http_error_status::UNDEFINED)
	, chunked_status_(chunked_status::UNDEFINED)
	, chunked_size_(0) {}

HttpRequest::~HttpRequest(){};

HttpRequest::HttpRequest(HttpRequest const& request) {
	*this = request;
}

HttpRequest& HttpRequest::operator=(HttpRequest const& request) {
	if (this != &request) {
		status_ = request.status_;
		method_ = request.method_;
		version_ = request.version_;
		error_status_ = request.error_status_;
		chunked_status_ = request.chunked_status_;
		chunked_size_ = request.chunked_size_;
		request_path_ = request.request_path_;
		header_ = request.header_;
		body_ = request.body_;
	}
	return (*this);
}

void HttpRequest::setStatus(http_request_status::HTTP_REQUEST_STATUS const& status) {
	status_ = status;
}

void HttpRequest::setErrorStatus(http_error_status::HTTP_ERROR_STATUS const& error_status) {
	error_status_ = error_status;
}
int HttpRequest::setMethod(std::string const& method) {
	if (method == "GET")
		method_ = http_method::GET;
	else if (method == "POST")
		method_ = http_method::POST;
	else if (method == "DELETE")
		method_ = http_method::DELETE;
	else {
		setStatus(http_request_status::ERROR);
		return (-1);
	}
	return (0);
}

int HttpRequest::setRequestPath(std::string const& request_path) {
	if (request_path == "\0") {
		setStatus(http_request_status::ERROR);
		return (-1);
	}
	request_path_ = request_path;
	return (0);
}

int HttpRequest::setVersion(std::string const& version) {
	if (version == "HTTP/1.0")
		version_ = http_version::HTTP_1_0;
	else if (version == "HTTP/1.1")
		version_ = http_version::HTTP_1_1;
	else {
		setStatus(http_request_status::ERROR);
		return (-1);
	}
	return (0);
}

int HttpRequest::parseHttpRequest(std::string const& line) {
	switch (status_) {
		case http_request_status::METHOD:
			parseHttpMethod(line);
			break;
		case http_request_status::HEADER:
			parseHttpHeader(line);
			break;
		case http_request_status::BODY:
			parseHttpBody(line);
			break;
		default:
			setStatus(http_request_status::ERROR);
			break;
	}
	if (status_ == http_request_status::ERROR)
		return (-1);
	return (0);
}

int HttpRequest::parseHttpMethod(std::string const& line) {
	std::vector<std::string> method_vector;
	if (parseSentense(line, "%s %s %s", method_vector) == -1 || method_vector.size() != 3) {
		std::cerr << "Http Method Parse Error" << std::endl;
		setStatus(http_request_status::ERROR);
		setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	if (setMethod(method_vector[0]) < 0 || setRequestPath(method_vector[1]) < 0 ||
		setVersion(method_vector[2]) < 0) {
		std::cerr << "Http Method Parse Error" << std::endl;
		setStatus(http_request_status::ERROR);
		setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	setStatus(http_request_status::HEADER);
	return (0);
}

int HttpRequest::parseHttpHeader(std::string const& line) {
	std::vector<std::string> header_vector;
	if (line == "\0") {
		if (method_ == http_method::GET || getHeaderValue("Content-Length") == "0")
			setStatus(http_request_status::FINISHED);
		else
			setStatus(http_request_status::BODY);
		return (0);
	}
	// TODO Later: http request compromise 0 space between : and value!
	else if (parseSentense(line, "%s: %s", header_vector) < 0 || header_vector.size() != 2) {
		setStatus(http_request_status::ERROR);
		setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	if (setHeaderValue(header_vector[0], header_vector[1]) < 0) {
		setStatus(http_request_status::ERROR);
		setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	return (0);
}

int HttpRequest::setHeaderValue(std::string const& key, std::string const& value) {
	header_.insert(std::make_pair(key, value));
	return (0);
}

std::string HttpRequest::getHeaderValue(std::string const& key) {
	if (header_.count(key) == 0)
		return (std::string(""));
	else
		return (header_[key]);
}

int HttpRequest::parseHttpBody(std::string const& line) {
	if (getHeaderValue("Transfer-Encoding") == "chunked")
		return (parseChunkedBody(line));
	else if (getHeaderValue("Content-Length") != "")
		return (parseContentLengthBody(line));
	setStatus(http_request_status::ERROR);
	setErrorStatus(http_error_status::BAD_REQUEST);
	return (-1);
}

int HttpRequest::parseChunkedBody(std::string const& line) {
	if (chunked_status_ == chunked_status::CHUNKED_SIZE) {
		// TODO no error handling
		// TODO Caution ! strtol is C++ 11 function.
		chunked_size_ = std::strtol(line.c_str(), NULL, 16);
		chunked_status_ = chunked_status::CHUNKED_MESSAGE;
		if (chunked_size_ == 0)
			setStatus(http_request_status::FINISHED);
	} else if (line == "")
		chunked_status_ = chunked_status::CHUNKED_SIZE;
	else
		body_ += line;
	return (0);
}

int HttpRequest::parseContentLengthBody(std::string const& line) {
	// TODO  no error handling
	body_ += line;
	if (body_.size() == static_cast<size_t>(std::atoi(getHeaderValue("Content-Length").c_str())))
		setStatus(http_request_status::FINISHED);
	return (0);
}

void HttpRequest::getInfo(void) {
	std::cout << "method: " << method_ << std::endl;
	std::cout << "status: " << status_ << std::endl;
	std::cout << "version: " << version_ << std::endl;
	std::cout << "header" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = header_.begin(); iter != header_.end();
		 ++iter)
		std::cout << "key: " << iter->first << "value: " << iter->second << std::endl;
}

http_request_status::HTTP_REQUEST_STATUS HttpRequest::getHttpRequestStatus(void) {
	return (status_);
}
}