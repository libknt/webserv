#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse()
	: response_() {}

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

void	HttpResponse::setFilePath(std::string const &file_path)
{
	file_path_ = file_path;
}

std::string const &HttpResponse::getFilePath(void)
{
	return (file_path_);
}

void	HttpResponse::insertStream(HttpRequest const &request)
{
	stream_ = request.getServerProtocol() + " " + std::to_string(status_code_) + " " + "OK" + "\r\n";
	for (std::map<std::string, std::string>::iterator iter = header_.begin(); iter != header_.end(); iter++)
		stream_ += (iter->first + ": " + iter->second + "\r\n");
	stream_ += "\r\n";
}

void	HttpResponse::addStream(std::string const &buf)
{
	stream_ += buf;
}

RequestProcessStatus HttpResponse::setSendBuffer2(char* buffer, size_t &buffer_size, size_t const &max_buffer_size) {
	std::memset(buffer, 0, max_buffer_size);
	if (stream_.size() <= max_buffer_size)
	{
		std::memcpy(buffer, stream_.c_str(), stream_.size());
		buffer_size = stream_.size();
		return FINISH;
	}
	else
	{
		std::string temp = stream_.substr(0, max_buffer_size);		
		std::memcpy(buffer, stream_.c_str(), max_buffer_size);
		buffer_size = max_buffer_size;
		stream_ = stream_.substr(max_buffer_size);
		return (SEND);
	}
}
} // namespace server
