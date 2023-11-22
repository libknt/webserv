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

void ClientSession::setStatus(CLIENT_SESSION_STATUS const& status) {
	status_ = status;
}

CLIENT_SESSION_STATUS ClientSession::getStatus() const {
	return status_;
}

void ClientSession::setSessionStatusFromHttpRequest() {
	http_request_status::HTTP_REQUEST_STATUS request_status = request_.getStatus();

	switch (request_status) {
		case http_request_status::FINISHED:
			setStatus(RESPONSE_PREPARING);
			break;
		case http_request_status::ERROR:
			setStatus(ERROR_OCCURRED);
			break;
		case http_request_status::UNDEFINED:
			setStatus(ERROR_OCCURRED);
			break;
		default:
			setStatus(REQUEST_RECEIVING);
			break;
	}
}

}