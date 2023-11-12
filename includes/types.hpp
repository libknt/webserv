#ifndef TYPES_HPP
#define TYPES_HPP

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
