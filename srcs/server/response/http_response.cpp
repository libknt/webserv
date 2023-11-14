#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& other)
	: status_code_(other.status_code_)
	, header_(other.header_)
	, body_(other.body_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		status_code_ = other.status_code_;
		header_ = other.header_;
		body_ = other.body_;
	}
	return *this;
}

const std::string HttpResponse::createResponse() {
	std::string response;
	std::stringstream stringstream;
	stringstream << status_code_;

	response += "HTTP/1.1 " + stringstream.str() + " " + statusCodeToStatusText(status_code_) + "\r\n";
	for (std::map<std::string, std::string>::iterator it = header_.begin(); it != header_.end(); ++it) {
		response += it->first + it->second + "\r\n";
	}
	response += "/r/n";
	response += body_;
	return response;
}

std::string HttpResponse::statusCodeToStatusText(const STATUS_CODE code) {
    switch (code) {
        case OK:
            return "OK";
        case CREATED:
            return "CREATED";
        case PERMANENT_REDIRECT:
            return "PERMANENT_REDIRECT";
        case BAD_REQUEST:
            return "BAD_REQUEST";
        case NOT_FOUND:
            return "NOT_FOUND";
        default:
            return "UNKNOWN";
    }
}

void HttpResponse::setStatusCode(const STATUS_CODE& status_code) {
	status_code_ = status_code;
}

void HttpResponse::setHeaderValue(const std::string &key, const std::string &value) {
	header_.insert(std::make_pair(key, value));
}

void HttpResponse::setBody(const std::string &body) {
	body_ = body;
}

STATUS_CODE HttpResponse::getStatusCode() const {
	return status_code_;
}

const std::string HttpResponse::getHeaderValue(const std::string &key) {
	return header_[key];
}

const std::map<std::string, std::string> &HttpResponse::getHeader() const{
	return header_;
}

const std::string &HttpResponse::getBody() const{
	return body_;
}

} // namespace server