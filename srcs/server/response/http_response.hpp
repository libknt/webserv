#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "struct.hpp"
#include <iostream>

namespace server {

class HttpResponse {
private:
	std::string response_;

public:
	HttpResponse();
	~HttpResponse();
	int initialize();
	RequestProcessStatus setSendBuffer(char* buffer, size_t max_buffer_size);
	std::string substr_response(size_t size);
};

} // namespace server

#endif