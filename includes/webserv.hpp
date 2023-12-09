#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sstream>
#include <string>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

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
} // namespace http_status_code

#endif