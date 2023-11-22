#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "http_request.hpp"
#include <arpa/inet.h>
#include <iostream>

namespace server {

class ClientSession {
private:
	const int sd_;
	HttpRequest request_;

	/* data */
	ClientSession();

public:
	explicit ClientSession(int const sd,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address);
	~ClientSession();
	ClientSession(const ClientSession& other);
	ClientSession& operator=(const ClientSession& other);
};

}

#endif