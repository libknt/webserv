#include "http_request.hpp"
#include <cstdlib>

namespace server {

HttpRequest::HttpRequest()
	: status_(http_request_status::METHOD)
	, method_(http_method::UNDEFINED)
	, version_(http_version::UNDEFINED)
	, body_message_type_(http_body_message_type::UNDEFINED)
	, content_length_(0)
	, chunked_status_(chunked_status::CHUNKED_SIZE)
	, chunked_size_(0) {}

HttpRequest::~HttpRequest(){};

HttpRequest::HttpRequest(HttpRequest const& other)
	: status_(other.status_)
	, method_(other.method_)
	, uri_(other.uri_)
	, version_(other.version_)
	, header_(other.header_)
	, body_message_type_(other.body_message_type_)
	, content_length_(other.content_length_)
	, chunked_status_(other.chunked_status_)
	, chunked_size_(other.chunked_size_)
	, body_(other.body_) {}

HttpRequest& HttpRequest::operator=(HttpRequest const& other) {
	if (this != &other) {
		status_ = other.status_;
		method_ = other.method_;
		uri_ = other.uri_;
		version_ = other.version_;
		header_ = other.header_;
		body_message_type_ = other.body_message_type_;
		content_length_ = other.content_length_;
		chunked_status_ = other.chunked_status_;
		chunked_size_ = other.chunked_size_;
		body_ = other.body_;
	}
	return (*this);
}

std::string const& HttpRequest::getStreamLine() const {
	return (stream_line_);
}

http_request_status::HTTP_REQUEST_STATUS const& HttpRequest::getStatus() const {
	return (status_);
}

http_error_status::HTTP_ERROR_STATUS const& HttpRequest::getErrorStatus() const {
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
		default:
			protocol = std::string("");
	}
	return protocol;
}

std::string const& HttpRequest::getUri() const {
	return uri_;
}

std::string const HttpRequest::getHeaderValue(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = header_.find(key);
	if (it == header_.end())
		return (std::string(""));
	return it->second;
}

std::map<std::string, std::string> const& HttpRequest::getHeader() const {
	return header_;
}

http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& HttpRequest::getBodyMessageType() const {
	return (body_message_type_);
}

size_t const& HttpRequest::getContentLength() const {
	return (content_length_);
}

chunked_status::CHUNKED_STATUS const& HttpRequest::getChunkedStatus() const {
	return (chunked_status_);
}

size_t HttpRequest::getChunkedSize() const {
	return (chunked_size_);
}

std::string const& HttpRequest::getBody() const {
	return body_;
}

size_t HttpRequest::getBodySize() const {
	return (body_.size());
}

std::string HttpRequest::getUriPath() const {
	std::string::size_type query_index = uri_.find("?");
	if (query_index != std::string::npos) {
		return (uri_.substr(0, query_index));
	}
	return (uri_);
}

std::string HttpRequest::getUriQuery() const {
	std::string::size_type query_index = uri_.find("?");
	if (query_index != std::string::npos && query_index + 1 <= uri_.size()) {
		return (uri_.substr(query_index + 1));
	}
	return (std::string(""));
}

void HttpRequest::appendStreamLine(std::string const& stream_line) {
	stream_line_ += stream_line;
}

void HttpRequest::setStreamLine(std::string const& stream_line) {
	stream_line_ = stream_line;
}

void HttpRequest::eraseStreamLine(std::string::size_type position, std::string::size_type n) {
	stream_line_.erase(position, n);
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

int HttpRequest::setRequestPath(std::string const& uri) {
	if (uri.size() == 0) {
		setStatus(http_request_status::ERROR);
		return (-1);
	}
	uri_ = uri;
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
	std::string internal_key;
	for (size_t i = 0; i < key.size(); i++) {
		if (!isTokenCharacter(key[i])) {
			setStatus(http_request_status::ERROR);
			return (-1);
		}
		const char chr = std::tolower(key[i]);
		internal_key.push_back(chr);
	}
	std::map<std::string, std::string>::iterator it = header_.find(internal_key);
	if (it != header_.end()) {
		it->second = it->second + ", " + value;
	} else
		header_.insert(std::make_pair(internal_key, value));
	return (0);
}

void HttpRequest::setBodyMassageType(
	http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& body_message_type) {
	body_message_type_ = body_message_type;
}

void HttpRequest::setContentLength(size_t content_length) {
	content_length_ = content_length;
}

void HttpRequest::setChunkedStatus(chunked_status::CHUNKED_STATUS const& chunked_status) {
	chunked_status_ = chunked_status;
}

void HttpRequest::setChunkedSize(size_t chunked_size) {
	chunked_size_ = chunked_size;
}

void HttpRequest::appendBody(std::string const& body) {
	body_ += body;
}

bool HttpRequest::isTokenDelimiter(char chr) {
	return (chr == '(' || chr == ')' || chr == ',' || chr == '/' || chr == ':' || chr == ';' ||
			chr == '<' || chr == '>' || chr == '=' || chr == '?' || chr == '@' || chr == '[' ||
			chr == '\\' || chr == ']' || chr == '{' || chr == '}');
}

bool HttpRequest::isTokenCharacter(char chr) {
	return (std::isdigit(chr) || std::isalpha(chr) ||
			(std::isprint(chr) && !isTokenDelimiter(chr) && chr != '"'));
}

std::ostream& operator<<(std::ostream& out, const HttpRequest& request) {

	out << "method: " << request.getMethod() << std::endl;
	out << "request path: " << request.getUri() << std::endl;
	out << "status: " << request.getStatus() << std::endl;
	out << "version: " << request.getVersion() << std::endl;
	out << "header" << std::endl;

	std::map<std::string, std::string> header = request.getHeader();
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end();
		 ++it) {
		std::cout << "key: " << it->first << " value: " << it->second << std::endl;
	}
	std::cout << "-----body----" << std::endl;
	std::cout << request.getBody() << std::endl;
	return out;
}

}
