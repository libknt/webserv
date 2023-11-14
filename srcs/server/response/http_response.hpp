#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "types.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

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
	STATUS_CODE status_code_;
	std::map<std::string, std::string> header_;
	std::string body_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	const std::string createResponse();
	std::string statusCodeToStatusText(const STATUS_CODE code);
	void setStatusCode(const STATUS_CODE& status_code);
	void setHeaderValue(const std::string &key, const std::string &value);
	void setBody(const std::string &body);
	STATUS_CODE getStatusCode() const;
	const std::string getHeaderValue(const std::string &key);
	const std::map<std::string, std::string> &getHeader() const;
	const std::string &getBody() const;
};

} // namespace server

#endif