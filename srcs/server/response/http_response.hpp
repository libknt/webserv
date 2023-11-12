#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "types.hpp"
#include <cstring>
#include <iostream>

namespace server {

class HttpResponse {
private:
	std::string response_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	const std::string& getResponse() const;
};

} // namespace server

#endif