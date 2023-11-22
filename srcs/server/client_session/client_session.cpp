#include "client_session.hpp"

namespace server {

ClientSession::ClientSession(int const sd,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: sd_(sd)
	, request_(HttpRequest(client_address, server_address)) {}

ClientSession::~ClientSession() {}

ClientSession::ClientSession(const ClientSession& other)
	: sd_(other.sd_)
	, request_(other.request_) {}

ClientSession& ClientSession::operator=(const ClientSession& other) {
	if (this != &other) {
		request_ = other.request_;
	}
	return *this;
}

}