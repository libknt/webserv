#include "client_session.hpp"

namespace server {

ClientSession::ClientSession(sockaddr_in client_address, sockaddr_in server_address)
	: request_(HttpRequest(client_address, server_address)) {}

ClientSession::~ClientSession() {}

ClientSession::ClientSession(const ClientSession& other)
	: request_(other.request_) {}

ClientSession& ClientSession::operator=(const ClientSession& other) {
	if (this != &other) {
		request_ = other.request_;
	}
	return *this;
}

}