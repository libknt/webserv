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
	CGI_SEND_BODY,
	CGI_EXEC,
	SEND,
	ERROR,
	UNDEFINED,
};
}
#endif