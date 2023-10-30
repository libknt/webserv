#ifndef STRUCT_HPP
#define STRUCT_HPP

struct socket_conf {
	const char* addr;
	int port;
};

namespace server {
enum RequestProcessStatus {
	RECV,
	REQUESTFINISH,
	RESPONSE,
	CGI,
	SEND,
	ERROR,
	UNDEFINED,
};
}
#endif