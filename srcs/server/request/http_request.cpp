#include "http_request.hpp"
#include "parse_sentense.hpp"
#include <cstdlib>

namespace server {

HttpRequest::HttpRequest(sockaddr_in client_address, sockaddr_in server_address)
	: status_(http_request_status::METHOD)
	, error_status_(http_error_status::UNDEFINED)
	, method_(http_method::UNDEFINED)
	, version_(http_version::UNDEFINED)
	, body_message_type_(http_body_message_type::UNDEFINED)
	, content_length_(0)
	, chunked_status_(chunked_status::CHUNKED_SIZE)
	, chunked_size_(0)
	, client_address_(client_address)
	, server_address_(server_address)
	, is_cgi_(false) {}

HttpRequest::~HttpRequest(){};

HttpRequest::HttpRequest(HttpRequest const& other)
	: status_(other.status_)
	, error_status_(other.error_status_)
	, method_(other.method_)
	, request_path_(other.request_path_)
	, version_(other.version_)
	, header_(other.header_)
	, body_message_type_(other.body_message_type_)
	, content_length_(other.content_length_)
	, chunked_status_(other.chunked_status_)
	, chunked_size_(other.chunked_size_)
	, body_(other.body_)
	, client_address_(other.client_address_)
	, server_address_(other.server_address_)
	, is_cgi_(other.is_cgi_) {}

HttpRequest& HttpRequest::operator=(HttpRequest const& other) {
	if (this != &other) {
		status_ = other.status_;
		error_status_ = other.error_status_;
		method_ = other.method_;
		request_path_ = other.request_path_;
		version_ = other.version_;
		header_ = other.header_;
		body_message_type_ = other.body_message_type_;
		content_length_ = other.content_length_;
		chunked_status_ = other.chunked_status_;
		chunked_size_ = other.chunked_size_;
		body_ = other.body_;
		client_address_ = other.client_address_;
		server_address_ = other.server_address_;
		is_cgi_ = other.is_cgi_;
	}
	return (*this);
}

int HttpRequest::parseHttpRequest(std::string const& line) {
	switch (status_) {
		case http_request_status::METHOD:
			parseMethod(line);
			break;
		case http_request_status::HEADER:
			parseHeader(line);
			break;
		case http_request_status::BODY:
			parseBody(line);
			break;
		default:
			setStatus(http_request_status::ERROR);
			break;
	}
	if (status_ == http_request_status::ERROR)
		return (-1);
	return (0);
}

http_request_status::HTTP_REQUEST_STATUS HttpRequest::getStatus(void) const {
	return (status_);
}

std::string HttpRequest::getMethod() const {
	std::string method;
	switch (method_) {
		case http_method::GET:
			method = "GET";
			break;
		case http_method::POST:
			method = "POST";
			break;
		case http_method::DELETE:
			method = "DELETE";
			break;
		default:
			method = std::string("");
	}
	return method;
}

std::string HttpRequest::getVersion() const {
	std::string protocol;
	switch (version_) {
		case http_version::HTTP_1_0:
			protocol = "HTTP_1_0";
			break;
		case http_version::HTTP_1_1:
			protocol = "HTTP_1_1";
			break;
		case http_version::HTTP_2_0:
			protocol = "HTTP_2_0";
			break;
		default:
			protocol = "";
	}
	return protocol;
}

std::string HttpRequest::getRequestPath() const {
	return request_path_;
}

std::string HttpRequest::getHeaderValue(std::string const& key) {
	if (header_.empty())
		return (std::string(""));
	else
		return (header_[key]);
}

http_body_message_type::HTTP_BODY_MESSAGE_TYPE HttpRequest::getBodyMessageType(void) {
	return (body_message_type_);
}

std::map<std::string, std::string> HttpRequest::getHeader() const {
	return header_;
}

std::string HttpRequest::getBody() const {
	return body_;
}

sockaddr_in HttpRequest::getClientAddress() const {
	return client_address_;
}

sockaddr_in HttpRequest::getServerAddress() const {
	return server_address_;
}

std::string HttpRequest::getServerIpAddress() const {
	sockaddr_in server_address = getServerAddress();
	char* server_ip = inet_ntoa(server_address.sin_addr);
	return (std::string(server_ip));
}

std::string HttpRequest::getServerPort() const {
	sockaddr_in server_address = getServerAddress();
	unsigned short server_port = ntohs(server_address.sin_port);

	std::stringstream ss;
	ss << server_port;
	return ss.str();
}

void HttpRequest::setStatus(http_request_status::HTTP_REQUEST_STATUS const& status) {
	status_ = status;
}

void HttpRequest::setErrorStatus(http_error_status::HTTP_ERROR_STATUS const& error_status) {
	error_status_ = error_status;
}

void HttpRequest::setIsCgi(bool is_cgi) {
	is_cgi_ = is_cgi;
}

bool HttpRequest::getIsCgi() const {
	return is_cgi_;
}

int HttpRequest::parseMethod(std::string const& line) {
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

int HttpRequest::parseHeader(std::string const& line) {
	std::vector<std::string> header_vector;
	if (line == "\0") {
		if (checkHeaderValue() < 0) {
			setStatus(http_request_status::ERROR);
			setErrorStatus(http_error_status::BAD_REQUEST);
			return (-1);
		}
		if (method_ == http_method::GET || method_ == http_method::DELETE)
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

int HttpRequest::checkHeaderValue() {
	switch (method_) {
		case http_method::GET:
		case http_method::DELETE:
			body_message_type_ = http_body_message_type::NONE;
			break;
		case http_method::POST:
			if (getHeaderValue("Transfer-Encoding") == "chunked")
				body_message_type_ = http_body_message_type::CHUNK_ENCODING;
			else if (getHeaderValue("Content-Length") != "") {
				body_message_type_ = http_body_message_type::CONTENT_LENGTH;
				// TODO you should check the value is affordable.
				content_length_ =
					static_cast<size_t>(std::atoi(getHeaderValue("Content-Length").c_str()));
			}
			break;
		default:
			setStatus(http_request_status::ERROR);
			setErrorStatus(http_error_status::BAD_REQUEST);
			return (-1);
	}
	return (0);
}

int HttpRequest::parseBody(std::string const& line) {
	switch (body_message_type_) {
		case http_body_message_type::CHUNK_ENCODING:
			return (parseChunkedBody(line));
		case http_body_message_type::CONTENT_LENGTH:
			return (parseContentLengthBody(line));
		default:
			setStatus(http_request_status::ERROR);
			setErrorStatus(http_error_status::BAD_REQUEST);
			return (-1);
	}
}

int HttpRequest::parseContentLengthBody(std::string const& line) {
	// TODO  no error handling
	body_ += line;
	if (body_.size() == content_length_)
		setStatus(http_request_status::FINISHED);
	return (0);
}

int HttpRequest::parseChunkedBody(std::string const& line) {
	if (chunked_status_ == chunked_status::CHUNKED_SIZE) {
		// TODO no error handling
		// TODO Caution ! strtol is C++ 11 function.
		chunked_size_ = std::strtol(line.c_str(), NULL, 16);
		chunked_status_ = chunked_status::CHUNKED_MESSAGE;
	} else if (line == "" && chunked_size_ == 0) {
		setStatus(http_request_status::FINISHED);
	} else {
		body_ += line;
		chunked_status_ = chunked_status::CHUNKED_SIZE;
	}
	return (0);
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

int HttpRequest::setHeaderValue(std::string const& key, std::string const& value) {
	header_.insert(std::make_pair(key, value));
	return (0);
}

std::ostream& operator<<(std::ostream& out, const HttpRequest& request) {

	sockaddr_in client_address = request.getClientAddress();
	sockaddr_in server_address = request.getServerAddress();
	char* client_ip = inet_ntoa(client_address.sin_addr);
	unsigned short client_port = ntohs(client_address.sin_port);
	char* server_ip = inet_ntoa(server_address.sin_addr);
	unsigned short server_port = ntohs(server_address.sin_port);
	out << "client ip: " << client_ip << std::endl;
	out << "client port: " << client_port << std::endl;
	out << "server ip: " << server_ip << std::endl;
	out << "server port: " << server_port << std::endl;
	out << "method: " << request.getMethod() << std::endl;
	out << "request path: " << request.getRequestPath() << std::endl;
	out << "status: " << request.getStatus() << std::endl;
	out << "version: " << request.getVersion() << std::endl;
	out << "header" << std::endl;

	std::map<std::string, std::string> header = request.getHeader();
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end();
		 ++it) {
		std::cout << "key: " << it->first << " value: " << it->second << std::endl;
	}
	return out;
}

void HttpRequest::cleanup() {
	status_ = http_request_status::METHOD;
	error_status_ = http_error_status::UNDEFINED;
	method_ = http_method::UNDEFINED;
	version_ = http_version::UNDEFINED;
	body_message_type_ = http_body_message_type::UNDEFINED;
	content_length_ = 0;
	chunked_status_ = chunked_status::CHUNKED_SIZE;
	chunked_size_ = 0;
}

}