#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "http_request.hpp"
#include <arpa/inet.h>
#include <iostream>

namespace server {

enum CLIENT_SESSION_STATUS {
	AWAITING_REQUEST,
	REQUEST_RECEIVING,
	RESPONSE_PREPARING,
	SESSION_COMPLETE,
	ERROR_OCCURRED,
};

class ClientSession {
private:
	const int sd_;
	HttpRequest request_;

	CLIENT_SESSION_STATUS status_;

	/* data */
	ClientSession();

public:
	explicit ClientSession(int const sd,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address);
	~ClientSession();
	ClientSession(const ClientSession& other);
	ClientSession& operator=(const ClientSession& other);
	int getSd() const;
	HttpRequest& getRequest();
	CLIENT_SESSION_STATUS getStatus() const;
};

}

#endif