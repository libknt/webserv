#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "http_request.hpp"
#include <arpa/inet.h>
#include <iostream>

namespace server {

class ClientSession {
private:
	HttpRequest request_;

	/* data */
	ClientSession();

public:
	explicit ClientSession(sockaddr_in client_address, sockaddr_in server_address);
	~ClientSession();
	ClientSession(const ClientSession& other);
	ClientSession& operator=(const ClientSession& other);
};

}

#endif