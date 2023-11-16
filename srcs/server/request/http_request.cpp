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
	, server_address_(server_address) {}

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
	, server_address_(other.server_address_) {}

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
	}
	return (*this);
}

http_request_status::HTTP_REQUEST_STATUS const& HttpRequest::getStatus(void) const {
	return (status_);
}

http_error_status::HTTP_ERROR_STATUS const& HttpRequest::getErrorStatus(void) const {
	return (error_status_);
}

std::string const HttpRequest::getMethod() const {
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

std::string const HttpRequest::getVersion() const {
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
			protocol = std::string("");
	}
	return protocol;
}

std::string const& HttpRequest::getRequestPath() const {
	return request_path_;
}

std::string const HttpRequest::getHeaderValue(std::string const& key) const{
	std::map<std::string, std::string>::const_iterator it = header_.begin();
	if (it == header_.begin())
		return (std::string(""));
	return it->second;
}

http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& HttpRequest::getBodyMessageType() {
	return (body_message_type_);
}

std::map<std::string, std::string> const& HttpRequest::getHeader() const {
	return header_;
}

std::string const& HttpRequest::getBody() const {
	return body_;
}

sockaddr_in const& HttpRequest::getClientAddress() const {
	return client_address_;
}

sockaddr_in const& HttpRequest::getServerAddress() const {
	return server_address_;
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
	if (request_path.size() == 0) {
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

int HttpRequest::setHeaderValue(std::string const& key, std::string const& value) {

}

int HttpRequest::setBodyMassageType(http_body_message_type::HTTP_BODY_MESSAGE_TYPE const &body_message_type) {

}
int HttpRequest::setContentLength(size_t content_length) {

}
int HttpRequest::setChunkedStatus(chunked_status::CHUNKED_STATUS const &chunked_status ) {

}
int HttpRequest::setChunkedSize(size_t chunked_size_) {

}
int HttpRequest::setBody(std::string const &body) {

}
int HttpRequest::setClientAddress(sockaddr_in const &client_address) {

}
int HttpRequest::setServerAddress(sockaddr_in const &server_address) {

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

}