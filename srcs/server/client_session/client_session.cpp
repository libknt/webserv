#include "client_session.hpp"

namespace server {

ClientSession::ClientSession(int const sd,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address,
	ServerDirective const& server_directive)
	: sd_(sd)
	, client_address_(client_address)
	, server_address_(server_address)
	, server_directive_(server_directive)
	, request_(HttpRequest())
	, cgi_(Cgi(request_, client_address, server_address))
	, response_(HttpResponse())
	, status_(AWAITING_REQUEST) {}

ClientSession::ClientSession(int const sd,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address,
	ServerDirective const& server_directive,
	CLIENT_SESSION_STATUS const& status)
	: sd_(sd)
	, client_address_(client_address)
	, server_address_(server_address)
	, server_directive_(server_directive)
	, request_(HttpRequest())
	, cgi_(Cgi(request_, client_address, server_address))
	, response_(HttpResponse())
	, status_(status) {}

ClientSession::~ClientSession() {}

ClientSession::ClientSession(const ClientSession& other)
	: sd_(other.sd_)
	, client_address_(other.client_address_)
	, server_address_(other.server_address_)
	, server_directive_(other.server_directive_)
	, request_(other.request_)
	, cgi_(other.cgi_)
	, response_(other.response_)
	, status_(other.status_) {}

ClientSession& ClientSession::operator=(const ClientSession& other) {
	if (this != &other) {
		request_ = other.request_;
		cgi_ = other.cgi_;
		response_ = other.response_;
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

Cgi& ClientSession::getCgi() {
	return cgi_;
}

HttpResponse& ClientSession::getResponse() {
	return response_;
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
			setStatus(EVALUATING_RESPONSE_TYPE);
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

sockaddr_in const& ClientSession::getClientAddress() const {
	return client_address_;
}

sockaddr_in const& ClientSession::getServerAddress() const {
	return server_address_;
}

std::string ClientSession::getClientIpAddress() const {
	sockaddr_in client_address = client_address_;
	char* client_ip = inet_ntoa(client_address.sin_addr);
	return (std::string(client_ip));
}

std::string ClientSession::getServerIpAddress() const {
	sockaddr_in server_address = server_address_;
	char* server_ip = inet_ntoa(server_address.sin_addr);
	return (std::string(server_ip));
}

std::string ClientSession::getClientPort() const {
	sockaddr_in client_address = getClientAddress();
	unsigned short client_port = ntohs(client_address.sin_port);

	std::stringstream ss;
	ss << client_port;
	return ss.str();
}

std::string ClientSession::getServerPort() const {
	sockaddr_in server_address = getServerAddress();
	unsigned short server_port = ntohs(server_address.sin_port);

	std::stringstream ss;
	ss << server_port;
	return ss.str();
}

ServerDirective const& ClientSession::getServerDirective() const {
	return server_directive_;
}

void ClientSession::sessionCleanup() {
	request_ = HttpRequest();
	response_ = HttpResponse();
}

std::ostream& operator<<(std::ostream& out, const ClientSession& client_session) {
	out << "ClientSession: " << std::endl;
	out << "  sd: " << client_session.getSd() << std::endl;
	out << "  client_address: " << client_session.getClientIpAddress() << ":"
		<< client_session.getClientPort() << std::endl;
	out << "  server_address: " << client_session.getServerIpAddress() << ":"
		<< client_session.getServerPort() << std::endl;
	out << "  status: " << client_session.getStatus() << std::endl;
	// out << "  request: " << std::endl;
	// out << client_session.getRequest() << std::endl;
	// out << "  response: " << std::endl;
	// out << client_session.getResponse();
	return out;
}

}
