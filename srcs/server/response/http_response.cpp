#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse()
	: response_(
		  "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: "
		  "1098\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n    <head>\r\n\r\n    <title>Extended Simple "
		  "Response</title>\r\n</head>\r\n<body>\r\n\r\n    <h1>Hello, this is an extended simple "
		  "HTTP response!</h1>\r\n\r\n<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
		  "Vivamus lacinia odio vitae vestibulum. Donec in efficitur leo. In nisl ligula, "
		  "vulputate "
		  "id diam in, condimentum tincidunt sapien. Maecenas gravida velit vitae dolor finibus, "
		  "in "
		  "feugiat urna interdum. Vestibulum euismod euismod velit, nec feugiat lacus feugiat sit "
		  "amet. Vestibulum consectetur sit amet lacus a pellentesque. Duis tincidunt, est sed "
		  "sodales tincidunt, ipsum erat elementum massa, at lacinia arcu ex a est. Fusce ut "
		  "congue "
		  "metus, sed tincidunt urna.</p>\r\n\r\n<p>Quisque egestas eget lacus non condimentum. "
		  "Sed "
		  "at imperdiet dui, vel facilisis velit. Proin ac neque nec arcu commodo aliquet. Morbi "
		  "tincidunt turpis et tincidunt. Fusce id dui id libero aliquet sagittis a at libero. "
		  "Nulla "
		  "at libero pharetra, bibendum metus sed, malesuada metus. Cras sollicitudin, quam "
		  "pellentesque lobortis auctor, ante eros bibendum nunc, sed bibendum quam ex non metus. "
		  "Curabitur in pharetra odio, in efficitur leo.</p>\r\n</body>\r\n</html>") {}

HttpResponse::~HttpResponse() {}


RequestProcessStatus HttpResponse::setSendBuffer(char* buffer, size_t max_buffer_size) {
	std::memset(buffer, 0, max_buffer_size);
	std::string buf = substr_response(max_buffer_size - 1);

	std::memcpy(buffer, buf.c_str(), buf.size());
	if (buf.size() == max_buffer_size - 1)
		return SEND;

	return FINISH;
}

std::string HttpResponse::substr_response(size_t size) {
	std::string ans;
	if (response_.size() < size) {
		ans = response_.substr(0, response_.size());
		response_.clear();
	} else {
		ans = response_.substr(0, size);
		response_ = response_.substr(size + 1);
	}
	return ans;
}

void	HttpResponse::setHeaderValue(std::string const key, std::string const value)
{
	header_.insert(std::make_pair(key, value));
}
void	HttpResponse::setStatusCode(int status_code)
{
	status_code_ = status_code;
}
} // namespace server