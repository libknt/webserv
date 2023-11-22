#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include <arpa/inet.h>
#include <iostream>

namespace server {

enum CLIENT_SESSION_STATUS {
	AWAITING_REQUEST,
	REQUEST_RECEIVING,
	EVALUATING_RESPONSE_TYPE,
	RESPONSE_PREPARING,
	CGI_PREPARING,
	SENDING_RESPONSE,
	SENDING_CGI_RESPONSE,
	SESSION_COMPLETE,
	ERROR_OCCURRED,
};

class ClientSession {
private:
	const int sd_;
	HttpRequest request_;
	HttpResponse response_;

	CLIENT_SESSION_STATUS status_;

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
	HttpResponse& getResponse();
	void setStatus(CLIENT_SESSION_STATUS const& status);
	CLIENT_SESSION_STATUS getStatus() const;
	void setSessionStatusFromHttpRequest();
	void sessionCleanup();
};

}

#endif