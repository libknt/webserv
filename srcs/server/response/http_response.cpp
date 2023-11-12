#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& other)
	: response_(other.response_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		response_ = other.response_;
	}
	return *this;
}

void HttpResponse::setStatusCode(const STATUS_CODE& status_code) {
	status_code_ = status_code;
}

const std::string& HttpResponse::getResponse() const {
	return response_;
}

} // namespace server