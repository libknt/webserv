#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "types.hpp"
#include <cstring>
#include <iostream>

namespace server {

enum STATUS_CODE {
	OK = 200,
	CREATED = 201,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	NOT_FOUND = 404
};

class HttpResponse {
private:
	std::string response_;
	STATUS_CODE status_code_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	void setStatusCode(const STATUS_CODE& status_code);
	const std::string& getResponse() const;
};

} // namespace server

#endif