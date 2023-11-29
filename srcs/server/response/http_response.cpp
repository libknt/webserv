#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse()
	: status_code_(http_status_code::OK)
	, header_()
	, body_()
	, status_(http_response_status::UNDEFINED) {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& other)
	: status_code_(other.status_code_)
	, header_(other.header_)
	, body_(other.body_)
	, status_(other.status_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		status_code_ = other.status_code_;
		header_ = other.header_;
		body_ = other.body_;
	}
	return *this;
}

const std::string HttpResponse::concatenateComponents() {
	std::string response;
	std::stringstream stringstream;
	stringstream << status_code_;

	response +=
		"HTTP/1.1 " + stringstream.str() + " " + statusCodeToStatusText(status_code_) + "\r\n";
	for (std::map<std::string, std::string>::iterator it = header_.begin(); it != header_.end();
		 ++it) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += body_;
	return response;
}

std::string HttpResponse::statusCodeToStatusText(const http_status_code::STATUS_CODE code) {
	switch (code) {
		case http_status_code::OK:
			return "OK";
		case http_status_code::CREATED:
			return "CREATED";
		case http_status_code::NO_CONTENT:
			return "NO_CONTENT";
		case http_status_code::PERMANENT_REDIRECT:
			return "PERMANENT_REDIRECT";
		case http_status_code::BAD_REQUEST:
			return "BAD_REQUEST";
		case http_status_code::NOT_FOUND:
			return "NOT_FOUND";
		default:
			return "UNKNOWN";
	}
}

void HttpResponse::setStatusCode(const http_status_code::STATUS_CODE& status_code) {
	status_code_ = status_code;
}

void HttpResponse::setHeaderValue(const std::string& key, const std::string& value) {
	header_.insert(std::make_pair(key, value));
}

void HttpResponse::setBody(const std::string& body) {
	body_ += body;
}

http_status_code::STATUS_CODE HttpResponse::getStatusCode() const {
	return status_code_;
}

const std::string HttpResponse::getHeaderValue(const std::string& key) {
	return header_[key];
}

const std::map<std::string, std::string>& HttpResponse::getHeader() const {
	return header_;
}

const std::string& HttpResponse::getBody() const {
	return body_;
}

http_response_status::HTTP_RESPONSE_STATUS const& HttpResponse::getStatus() const {
	return status_;
}

} // namespace server
