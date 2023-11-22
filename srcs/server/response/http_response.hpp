#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "define.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

namespace server {

namespace http_status_code {
enum STATUS_CODE {
	OK = 200,
	CREATED = 201,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
};
}

namespace http_response_status {
enum HTTP_RESPONSE_STATUS {
	RESPONSE_SENDING,
	FINISHED,
	ERROR,
	UNDEFINED,
};
} // namespace http_response_status

class HttpResponse {
private:
	http_status_code::STATUS_CODE status_code_;
	std::map<std::string, std::string> header_;
	std::string body_;
	http_response_status::HTTP_RESPONSE_STATUS status_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	void createResponse();
	std::string sendResponse();
	http_response_status::HTTP_RESPONSE_STATUS getStatus() const;
};

} // namespace server

#endif