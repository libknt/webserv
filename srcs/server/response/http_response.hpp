#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "struct.hpp"
#include "http_request.hpp"
#include <cstring>
#include <iostream>
#include <map>

namespace server {

class HttpResponse {
private:
	int	status_code_;
	std::map<std::string, std::string>	header_;
	std::string	stream_;
	std::string	file_path_;
	std::string response_; //後で消すやつ。

public:
	HttpResponse();
	~HttpResponse();
	void setHeaderValue(std::string  const key, std::string const value);
	void setStatusCode(int status_code);
	void setFilePath(std::string const &file_path);
	std::string const &getFilePath(void);
	void insertStream(HttpRequest const &request);
	void addStream(std::string const &buf);
	RequestProcessStatus setSendBuffer(char* buffer, size_t max_buffer_size); //後で書き直す。
	RequestProcessStatus setSendBuffer2(char* buffer, size_t &buffer_size, size_t const &max_buffer_size);
	std::string substr_response(size_t size); //後で消す
};

} // namespace server

#endif