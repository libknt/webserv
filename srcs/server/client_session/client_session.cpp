#include "client_session.hpp"

namespace server {

ClientSession::ClientSession(int const sd,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: sd_(sd)
	, request_(HttpRequest(client_address, server_address))
	, status_(AWAITING_REQUEST) {}

ClientSession::~ClientSession() {}

ClientSession::ClientSession(const ClientSession& other)
	: sd_(other.sd_)
	, request_(other.request_)
	, status_(other.status_) {}

ClientSession& ClientSession::operator=(const ClientSession& other) {
	if (this != &other) {
		request_ = other.request_;
		status_ = other.status_;
	}
	return *this;
}

int ClientSession::getSd() const {
	return sd_;
}

HttpRequest& ClientSession::getRequest() {
	return request_;
}

CLIENT_SESSION_STATUS ClientSession::getStatus() const {
	return status_;
}

}