#include "cgi_handler.hpp"

namespace server {

namespace cgi_handler {

void handleCgiProcess(ClientSession& client_session) {
	cgi::CgiRequest& cgi = client_session.getCgi();
	const HttpRequest& request = client_session.getRequest();

	if (cgi.setup() < 0 || cgi.execute() < 0) {
		createErrorResponse(client_session.getResponse(),
			http_status_code::INTERNAL_SERVER_ERROR,
			client_session.findLocation());
		client_session.getResponse().concatenateComponents();
		client_session.setStatus(SENDING_RESPONSE);
		return;
	}
	cgi::CgiResponse& cgi_response = client_session.getCgiResponse();
	cgi_response.setSocketFd(0, cgi.getSocketFd(0));
	cgi_response.setSocketFd(1, cgi.getSocketFd(1));
	cgi_response.setPid(cgi.getPid());
	cgi_response.setStage(cgi::HEADERS_SENT);

	if (request.getHttpMethod() == http_method::POST) {
		client_session.setStatus(CGI_BODY_SENDING);
	} else {
		client_session.setStatus(CGI_RECEIVEING);
	}
}

void handleCgiResponse(ClientSession& client_session) {
	// cgi::CgiResponse const& cgi_response = client_session.getCgiResponse();
	HttpResponse& response = client_session.getResponse();
	const LocationDirective& location_directive = client_session.findLocation();
	// createResponseFromCgiResponse(cgi_response, response, location_directive);
	createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
	response.setStatus(http_response_status::RESPONSE_SENDING);
	response.concatenateComponents();
}

// void createResponseFromCgiResponse(cgi::CgiResponse const& cgi_response,
// 	HttpResponse& response,
// 	const LocationDirective& location_directive) {

// 	if (cgi_response.getHeaders().empty()) {
// 		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
// 		return;
// 	}
// 	createLocalRedirectResponse(cgi_response, response, location_directive);
// 	createDocumentResponse(cgi_response, response, location_directive);
// }

// int createLocalRedirectResponse(cgi::CgiResponse const& cgi_response,
// 	HttpResponse& response,
// 	const LocationDirective& location_directive) {}

// int createDocumentResponse(cgi::CgiResponse const& cgi_response,
// 	HttpResponse& response,
// 	const LocationDirective& location_directive) {

// 	createStatusCode(cgi_response, response);
// 	createContentType(cgi_response, response);
// 	if (cgi_response.getContentLength() > 0) {
// 		std::string body = cgi_response.getBody();
// 		std::string new_body;
// 		for (std::size_t i = 0; i < body.size(); i++) {
// 			if (body[i] == '\n') {
// 				new_body += "\r\n";
// 			} else {
// 				new_body += body[i];
// 			}
// 		}
// 		response.setHeaderValue("content-length", Utils::toString(new_body.size()));
// 		response.setBody(new_body);
// 	}
// 	return 0;
// }

// void createStatusCode(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
// 	std::string status = cgi_response.getHeaderValue("status");
// 	http_status_code::STATUS_CODE status_code = http_status_code::OK;
// 	if (status.empty()) {
// 		response.setStatusCode(status_code);
// 	} else {
// 		int number = 200;
// 		// todo error check
// 		std::istringstream iss(status);
// 		std::string tmp;
// 		while (iss >> tmp) {
// 			std::istringstream iss2(tmp);
// 			if (iss2 >> number) {
// 				break;
// 			}
// 		}
// 		status_code = HttpResponse::numberToStatusCode(number);
// 		response.setStatusCode(status_code);
// 	}
// }

// void createContentType(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
// 	std::string content_type = cgi_response.getHeaderValue("content-type");
// 	if (!content_type.empty()) {
// 		response.setHeaderValue("content-type", content_type);
// 	} else {
// 		response.setHeaderValue("content-type", "text/plain");
// 	}
// }

//########################

// if (!cgi_response.getHeaderValue("location").empty() &&
// 	cgi_response.getHeaderValue("status").empty()) {
// 	createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
// 	return;
// } else {
// 	createHeaderFiled(cgi_response, response);
// }
// if (!cgi_response.getHeaderValue("location").empty()) {
// 	if (locationAnalysis(cgi_response.getHeaderValue("location"), response) < 0) {
// 		createErrorResponse(
// 			response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
// 		return;
// 	}
// }

// if (cgi_response.getContentLength() > 0) {
// 	response.setHeaderValue("content-length", Utils::toString(cgi_response.getContentLength()));
// 	std::string body = cgi_response.getBody();
// 	std::string new_body;
// 	for (std::size_t i = 0; i < body.size(); i++) {
// 		if (body[i] == '\n') {
// 			new_body += "\r\n";
// 		} else {
// 			new_body += body[i];
// 		}
// 	}
// 	response.setBody(new_body);
// }
//########################
// int locationAnalysis(std::string location, HttpResponse& response) {
// 	if (location.empty()) {
// 		return -1;
// 	}
// 	if (location.compare(0, 4, "http") == 0 || location.compare(0, 5, "https") == 0 ||
// 		location.compare(0, 3, "ftp") == 0 || location.compare(0, 6, "mailto") == 0 ||
// 		location.compare(0, 4, "file") == 0 || location.compare(0, 3, "tel") == 0 ||
// 		location.compare(0, 4, "data") == 0) {
// 		response.setHeaderValue("location", location);
// 		return 0;
// 	} else if (location.compare(0, 1, "/") == 0) {
// 		response.setHeaderValue("location", location);
// 		return 0;
// 	}
// 	return -1;
// }

// void createHeaderFiled(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
// 	std::string content_type = cgi_response.getHeaderValue("content-type");
// 	if (!content_type.empty()) {
// 		response.setHeaderValue("content-type", content_type);
// 	} else {
// 		response.setHeaderValue("content-type", "text/plain");
// 	}
// }

// int createStatusCode(std::string const& status) {
// 	std::string status_code;
// 	int number = 200;
// 	std::istringstream iss(status);
// 	std::string tmp;
// 	while (iss >> tmp) {
// 		std::istringstream iss2(tmp);
// 		if (iss2 >> number) {
// 			break;
// 		}
// 	}
// 	return number;
// }

} // namespace handle_cgi

} // namespace server
