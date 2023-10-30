#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "struct.hpp"
#include <iostream>

namespace server {

class HttpResponse {
public:
	HttpResponse();
	~HttpResponse();
	int initialize();
	RequestProcessStatus finish();
};

} // namespace server

#endif