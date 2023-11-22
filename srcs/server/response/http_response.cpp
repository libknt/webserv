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
		status_ = other.status_;
	}
	return (*this);
}

void HttpResponse::createResponse() {
	std::string buffer = "HTTP/1.1 200 OK\r\n"
						 "Date: Wed, 09 Nov 2023 12:00:00 GMT\r\n"
						 "Server: MyServer\r\n"
						 "Content-Type: text/html; charset=UTF-8\r\n"
						 "Content-Length: 97\r\n"
						 "\r\n"
						 "<html>\r\n"
						 "<head>\r\n"
						 "<title>Simple Page</title>\r\n"
						 "</head>\r\n"
						 "<body>\r\n"
						 "<h1>Hello, World!</h1>\r\n"
						 "</body>\r\n"
						 "</html>\r\n";
	body_ = buffer;
}

std::string HttpResponse::sendResponse() {
	std::string tmp = body_.substr(0, BUFFER_SIZE - 1);
	body_ = body_.erase(0, BUFFER_SIZE - 1);
	if (body_.empty()) {
		status_ = http_response_status::FINISHED;
	}
	return body_;
}

http_response_status::HTTP_RESPONSE_STATUS HttpResponse::getStatus() const {
	return status_;
}

} // namespace server
