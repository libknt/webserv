#include "http_response.hpp"
#include "webserv.hpp"

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

void HttpResponse::concatenateComponents() {

	stream_ << "HTTP/1.1 " + Utils::toString(status_code_) + " " +
				   statusCodeToStatusText(status_code_) + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = header_.begin();
		 it != header_.end();
		 ++it) {
		stream_ << it->first + ": " + it->second + "\r\n";
	}
	stream_ << "\r\n";
	stream_ << body_;
}

std::string HttpResponse::statusCodeToStatusText(const http_status_code::STATUS_CODE code) {
	switch (code) {
		case http_status_code::OK:
			return "OK";
		case http_status_code::CREATED:
			return "CREATED";
		case http_status_code::NO_CONTENT:
			return "NO_CONTENT";
		case http_status_code::FOUND:
			return "FOUND";
		case http_status_code::SEE_OTHER:
			return "SEE_OTHER";
		case http_status_code::PERMANENT_REDIRECT:
			return "PERMANENT_REDIRECT";
		case http_status_code::BAD_REQUEST:
			return "BAD_REQUEST";
		case http_status_code::FORBIDDEN:
			return "FORBIDDEN";
		case http_status_code::NOT_FOUND:
			return "NOT_FOUND";
		case http_status_code::METHOD_NOT_ALLOWED:
			return "METHOD_NOT_ALLOWED";
		case http_status_code::INTERNAL_SERVER_ERROR:
			return "INTERNAL_SERVER_ERROR";
		case http_status_code::LENGTH_REQUIRED:
			return "LENGTH_REQUIRED";
		default:
			return "UNKNOWN";
	}
}

http_status_code::STATUS_CODE HttpResponse::numberToStatusCode(const int code) {
	switch (code) {
		case 200:
			return http_status_code::OK;
		case 201:
			return http_status_code::CREATED;
		case 204:
			return http_status_code::NO_CONTENT;
		case 302:
			return http_status_code::FOUND;
		case 303:
			return http_status_code::SEE_OTHER;
		case 308:
			return http_status_code::PERMANENT_REDIRECT;
		case 400:
			return http_status_code::BAD_REQUEST;
		case 403:
			return http_status_code::FORBIDDEN;
		case 404:
			return http_status_code::NOT_FOUND;
		case 405:
			return http_status_code::METHOD_NOT_ALLOWED;
		case 500:
			return http_status_code::INTERNAL_SERVER_ERROR;
		default:
			return http_status_code::INTERNAL_SERVER_ERROR;
	}
}

void HttpResponse::setStatus(const http_response_status::HTTP_RESPONSE_STATUS& status) {
	status_ = status;
}

void HttpResponse::setStatusCode(const http_status_code::STATUS_CODE& status_code) {
	status_code_ = status_code;
}

void HttpResponse::setHeaderValue(const std::string& key, const std::string& value) {
	header_.insert(std::make_pair(key, value));
}

void HttpResponse::setBody(const std::string& body) {
	body_ = body;
}

void HttpResponse::appendBody(const std::string& body) {
	body_ += body;
}

http_status_code::STATUS_CODE HttpResponse::getStatusCode() const {
	return status_code_;
}

const std::string HttpResponse::getFileContentType(std::string const& file_name) const {
	std::string::size_type dot_location = file_name.rfind(".");
	std::string file_extention;
	if (dot_location != std::string::npos)
		file_extention = file_name.substr(file_name.rfind("."));

	if (file_extention == ".html")
		return ("text/html");
	else if (file_extention == ".css")
		return ("text/css");
	else if (file_extention == ".js")
		return ("text/javascript");
	else if (file_extention == ".pdf")
		return ("application/pdf");
	else if (file_extention == ".png")
		return ("image/png");
	else
		return ("text/plain");
}

const std::string HttpResponse::getHeaderValue(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = header_.find(key);
	if (it == header_.end())
		return (std::string(""));
	return it->second;
}

const std::map<std::string, std::string>& HttpResponse::getHeader() const {
	return header_;
}

const std::string& HttpResponse::getBody() const {
	return body_;
}

std::string::size_type HttpResponse::getBodyLength() const {
	return (body_.size());
}

http_response_status::HTTP_RESPONSE_STATUS const& HttpResponse::getStatus() const {
	return status_;
}

void HttpResponse::getStreamBuffer(char* buffer, size_t buffer_size) {
	stream_.read(buffer, buffer_size);
	if (stream_.eof()) {
		setStatus(http_response_status::FINISHED);
		stream_.clear();
	}
}

std::ostream& operator<<(std::ostream& out, const HttpResponse& response) {
	out << "method: " << response.getStatusCode() << std::endl;
	std::map<std::string, std::string> header = response.getHeader();

	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end();
		 ++it) {
		std::cout << "key: " << it->first << " value: " << it->second << std::endl;
	}
	std::cout << "-----body----" << std::endl;
	std::cout << response.getBody() << std::endl;
	return out;
}

// std::ostream& operator<<(std::ostream& out, const HttpResponse& response) {
// 	out << "status_code: " << response.getStatusCode() << std::endl;
// 	out << "header: " << std::endl;
// 	for (std::map<std::string, std::string>::const_iterator it = response.getHeader().begin();
// 		 it != response.getHeader().end();
// 		 ++it) {
// 		out << it->first << ": " << it->second << std::endl;
// 	}
// 	out << "body: " << std::endl;
// 	out << response.getBody() << std::endl;
// 	return out;
// }

} // namespace server
