#ifndef TYPES_HPP
#define TYPES_HPP

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

namespace server {

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
