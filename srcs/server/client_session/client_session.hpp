#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include "server_directive.hpp"
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
	CLOSED,
};

class ClientSession {
private:
	const int sd_;
	const sockaddr_in client_address_;
	const sockaddr_in server_address_;

	ServerDirective const& server_directive_;
	HttpRequest request_;
	HttpResponse response_;
	CLIENT_SESSION_STATUS status_;

	ClientSession();

public:
	explicit ClientSession(int const sd,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address,
		ServerDirective const& server_directive);
	explicit ClientSession(int const sd,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address,
		ServerDirective const& server_directive,
		CLIENT_SESSION_STATUS const& status);
	~ClientSession();
	ClientSession(const ClientSession& other);
	ClientSession& operator=(const ClientSession& other);
	int getSd() const;
	HttpRequest& getRequest();
	HttpResponse& getResponse();
	sockaddr_in const& getClientAddress() const;
	sockaddr_in const& getServerAddress() const;
	std::string getClientIpAddress() const;
	std::string getServerIpAddress() const;
	std::string getClientPort() const;
	std::string getServerPort() const;
	void setStatus(CLIENT_SESSION_STATUS const& status);
	CLIENT_SESSION_STATUS getStatus() const;
	void setSessionStatusFromHttpRequest();
	void sessionCleanup();
};

std::ostream& operator<<(std::ostream& out, const ClientSession& client_session);

}

#endif