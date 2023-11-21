#ifndef TYPES_HPP
#define TYPES_HPP

namespace server {
static const int BUFFER_SIZE = 1024;

enum SERVER_STATUS {
	REQUEST,
	RESPONSE,
	CGI,
	FINISHED,
	ERROR,
	UNDEFINED,
};

} // namespace server

#endif
