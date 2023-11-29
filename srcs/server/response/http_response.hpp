#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "webserv.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

namespace server {

namespace http_status_code {
enum STATUS_CODE {
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_SERVER_ERROR = 500,
};
}

namespace http_response_status {
enum HTTP_RESPONSE_STATUS {
	RESPONSE_SENDING,
	FINISHED,
	ERROR,
	UNDEFINED,
};
}

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
	const std::string concatenateComponents();
	std::string statusCodeToStatusText(const http_status_code::STATUS_CODE code);
	void setStatusCode(const http_status_code::STATUS_CODE& status_code);
	void setHeaderValue(const std::string& key, const std::string& value);
	void setBody(const std::string& body);
	http_status_code::STATUS_CODE getStatusCode() const;
	const std::string getHeaderValue(const std::string& key);
	const std::map<std::string, std::string>& getHeader() const;
	const std::string& getBody() const;
	http_response_status::HTTP_RESPONSE_STATUS const& getStatus() const;
};

}

#endif