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

void HttpResponse::setStatusCode(const STATUS_CODE& status_code) {
	status_code_ = status_code;
}

const std::string HttpResponse::createResponse() {
	std::string response;

	response += statusCodeToString(status_code_) + "\r\n";
	for (std::map<std::string, std::string>::iterator it = header_.begin(); it != header_.end(); ++it) {
		response += it->first + it->second + "\r\n";
	}
	response += "/r/n";
	response += body_;
	return response;
}

std::string HttpResponse::statusCodeToString(const STATUS_CODE code) {
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

} // namespace server