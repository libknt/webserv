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
	std::string buffer =
		"HTTP/1.1 200 OK\r\n"
		"Date: Wed, 09 Nov 2023 12:00:00 GMT\r\n"
		"Server: MyServer\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: 1054\r\n"
		"\r\n"
		"<html>\r\n"
		"<head>\r\n"
		"<title>Simple Page</title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<h1>Hello, World!</h1>   \r\n"
		"<p>Welcome to our website! Here you can find various resources and information.</p>\r\n"
		"<h2>About Us</h2>\r\n"
		"<p>We are a team dedicated to providing the best content and services to our community. "
		"Our mission is to deliver informative and engaging content.</p>\r\n"
		"<h2>Features</h2>\r\n"
		"<ul>\r\n"
		"   <li>Quality Articles</li>\r\n"
		"   <li>Interactive Tutorials</li>\r\n"
		"   <li>Community Forums</li>\r\n"
		"   <li>Latest News and Updates</li>\r\n"
		"</ul>\r\n"
		"<h2>Gallery</h2>\r\n"
		"<img src=\"image1.jpg\" alt=\"Description of image 1\">\r\n"
		"<img src=\"image2.jpg\" alt=\"Description of image 2\">\r\n"
		"<img src=\"image3.jpg\" alt=\"Description of image 3\">\r\n"
		"<h2>Contact Us</h2>\r\n"
		"<p>Have questions or feedback? Reach out to us at <a "
		"href=\"mailto:contact@example.com\">contact@example.com</a>.</p>\r\n"
		"<footer>\r\n"
		"   <p>&copy; 2023 Our Website. All rights reserved.</p>\r\n"
		"</footer>\r\n"
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
	return tmp;
}

http_response_status::HTTP_RESPONSE_STATUS HttpResponse::getStatus() const {
	return status_;
}

} // namespace server
