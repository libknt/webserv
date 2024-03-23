#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "utils.hpp"
#include "webserv.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

namespace server {

// https://tex2e.github.io/rfc-translater/html/rfc2616.html
//    "100"  ; Section 10.1.1: Continue
//   | "101"  ; Section 10.1.2: Switching Protocols
//   | "200"  ; Section 10.2.1: OK
//   | "201"  ; Section 10.2.2: Created
//   | "202"  ; Section 10.2.3: Accepted
//   | "203"  ; Section 10.2.4: Non-Authoritative Information
//   | "204"  ; Section 10.2.5: No Content
//   | "205"  ; Section 10.2.6: Reset Content
//   | "206"  ; Section 10.2.7: Partial Content
//   | "300"  ; Section 10.3.1: Multiple Choices
//   | "301"  ; Section 10.3.2: Moved Permanently
//   | "302"  ; Section 10.3.3: Found
//   | "303"  ; Section 10.3.4: See Other
//   | "304"  ; Section 10.3.5: Not Modified
//   | "305"  ; Section 10.3.6: Use Proxy
//   | "307"  ; Section 10.3.8: Temporary Redirect
//   | "400"  ; Section 10.4.1: Bad Request
//   | "401"  ; Section 10.4.2: Unauthorized
//   | "402"  ; Section 10.4.3: Payment Required
//   | "403"  ; Section 10.4.4: Forbidden
//   | "404"  ; Section 10.4.5: Not Found
//   | "405"  ; Section 10.4.6: Method Not Allowed
//   | "406"  ; Section 10.4.7: Not Acceptable

//   | "407"  ; Section 10.4.8: Proxy Authentication Required
//   | "408"  ; Section 10.4.9: Request Time-out
//   | "409"  ; Section 10.4.10: Conflict
//   | "410"  ; Section 10.4.11: Gone
//   | "411"  ; Section 10.4.12: Length Required
//   | "412"  ; Section 10.4.13: Precondition Failed
//   | "413"  ; Section 10.4.14: Request Entity Too Large
//   | "414"  ; Section 10.4.15: Request-URI Too Large
//   | "415"  ; Section 10.4.16: Unsupported Media Type
//   | "416"  ; Section 10.4.17: Requested range not satisfiable
//   | "417"  ; Section 10.4.18: Expectation Failed
//   | "500"  ; Section 10.5.1: Internal Server Error
//   | "501"  ; Section 10.5.2: Not Implemented
//   | "502"  ; Section 10.5.3: Bad Gateway
//   | "503"  ; Section 10.5.4: Service Unavailable
//   | "504"  ; Section 10.5.5: Gateway Time-out
//   | "505"  ; Section 10.5.6: HTTP Version not supported
namespace http_status_code {
enum STATUS_CODE {
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	FOUND = 302,
	SEE_OTHER = 303,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_SERVER_ERROR = 500,
};
} // namespace http_status_code

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
	std::stringstream stream_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	void concatenateComponents();
	std::string statusCodeToStatusText(const http_status_code::STATUS_CODE code);
	static http_status_code::STATUS_CODE numberToStatusCode(const int code);
	void setStatus(const http_response_status::HTTP_RESPONSE_STATUS& status);
	void setStatusCode(const http_status_code::STATUS_CODE& status_code);
	void setHeaderValue(const std::string& key, const std::string& value);
	void setBody(const std::string& body);
	void appendBody(const std::string& body);
	http_status_code::STATUS_CODE getStatusCode() const;
	const std::string getFileContentType(std::string const& file_name) const;
	const std::string getHeaderValue(const std::string& key) const;
	const std::map<std::string, std::string>& getHeader() const;
	const std::string& getBody() const;
	std::string::size_type getBodyLength() const;
	http_response_status::HTTP_RESPONSE_STATUS const& getStatus() const;
	void getStreamBuffer(char* buffer, size_t buffer_size);
};

std::ostream& operator<<(std::ostream& out, const HttpResponse& response);
}

#endif