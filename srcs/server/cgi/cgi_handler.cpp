#include "cgi_handler.hpp"

namespace server {

namespace cgi_handler {

void handleCgiResponse(ClientSession& client_session) {
	cgi::CgiResponse const& cgi_response = client_session.getCgiResponse();
	HttpResponse& response = client_session.getResponse();
	const LocationDirective& location_directive = client_session.findLocation();
	createResponseFromCgiResponse(cgi_response, response, location_directive);
	response.setStatus(http_response_status::RESPONSE_SENDING);
	response.concatenateComponents();
}

void createResponseFromCgiResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive) {

	if (!cgi_response.getHeaderValue("location").empty() &&
		cgi_response.getHeaderValue("status").empty()) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return;
	} else {
		createHeaderFiled(cgi_response, response);
	}
	if (!cgi_response.getHeaderValue("location").empty()) {
		if (locationAnalysis(cgi_response.getHeaderValue("location"), response) < 0) {
			createErrorResponse(
				response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
			return;
		}
	}

	if (cgi_response.getContentLength() > 0) {
		response.setHeaderValue("content-length", Utils::toString(cgi_response.getContentLength()));
		response.setBody(cgi_response.getBody());
	}
}

int locationAnalysis(std::string location, HttpResponse& response) {
	if (location.empty()) {
		return -1;
	}
	if (location.compare(0, 4, "http") == 0 || location.compare(0, 5, "https") == 0 ||
		location.compare(0, 3, "ftp") == 0 || location.compare(0, 6, "mailto") == 0 ||
		location.compare(0, 4, "file") == 0 || location.compare(0, 3, "tel") == 0 ||
		location.compare(0, 4, "data") == 0) {
		response.setHeaderValue("location", location);
		return 0;
	} else if (location.compare(0, 1, "/") == 0) {
		response.setHeaderValue("location", location);
		return 0;
	}
	return -1;
}

void createHeaderFiled(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
	std::string status = cgi_response.getHeaderValue("status");
	http_status_code::STATUS_CODE status_code = http_status_code::OK;
	if (!status.empty()) {
		status_code = HttpResponse::numberToStatusCode(createStatusCode(status));
		response.setStatusCode(status_code);
	} else {
		response.setStatusCode(status_code);
	}

	std::string content_type = cgi_response.getHeaderValue("content-type");
	if (!content_type.empty()) {
		response.setHeaderValue("content-type", content_type);
	} else {
		response.setHeaderValue("content-type", "text/plain");
	}
}

int createStatusCode(std::string const& status) {
	std::string status_code;
	int number = 200;
	std::istringstream iss(status);
	std::string tmp;
	while (iss >> tmp) {
		std::istringstream iss2(tmp);
		if (iss2 >> number) {
			break;
		}
	}
	return number;
}

} // namespace handle_cgi

} // namespace server
