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
	cgi::CgiResponse const& cgi_response = client_session.getCgiResponse();
	HttpResponse& response = client_session.getResponse();
	const ServerDirective& server_directive = client_session.getServerDirective();
	const LocationDirective& location_directive = client_session.findLocation();
	createResponseFromCgiResponse(cgi_response, response, server_directive, location_directive);
	response.setStatus(http_response_status::RESPONSE_SENDING);
	response.concatenateComponents();
}

void createResponseFromCgiResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const ServerDirective& server_directive,
	const LocationDirective& location_directive) {

	if (cgi_response.getHeaders().empty()) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return;
	}
	if (createClientRedirectResponseWithDocument(cgi_response, response, location_directive) != 0) {
		std::cerr << "if (createClientRedirectResponseWithDocument(cgi_response, response, "
					 "location_directive) != 0) "
				  << std::endl;
		return;
	}
	if (createClientRedirectResponse(cgi_response, response, location_directive) != 0) {
		std::cerr << " if (createClientRedirectResponse(cgi_response, response, "
					 "location_directive) != 0) "
				  << std::endl;
		return;
	}
	if (createLocalRedirectResponse(cgi_response, response, server_directive, location_directive) !=
		0) {
		std::cerr << "if (createLocalRedirectResponse(cgi_response, response, server_directive, "
					 "location_directive) !="
				  << std::endl;
		return;
	}
	std::cerr << "createDocumentResponse::" << std::endl;
	createDocumentResponse(cgi_response, response, location_directive);
}

int locationAnalysis(std::string location) {
	if (location.empty()) {
		return -1;
	}
	if (location.compare(0, 7, "http://") == 0 || location.compare(0, 8, "https://") == 0 ||
		location.compare(0, 6, "ftp://") == 0 || location.compare(0, 9, "mailto://") == 0 ||
		location.compare(0, 7, "file://") == 0 || location.compare(0, 6, "tel://") == 0 ||
		location.compare(0, 7, "data://") == 0) {
		return 0;
	}
	return -1;
}

int createClientRedirectResponseWithDocument(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	if (cgi_response.isHeaderKeyPresent("location") == false) {
		return 0;
	}
	std::string location = cgi_response.getHeaderValue("location");
	if (location.empty()) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	if (locationAnalysis(location) != 0) {
		return 0;
	}
	std::string status = cgi_response.getHeaderValue("status");
	if (!status.empty() && !isRedirectStatus(status)) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	if (cgi_response.getHeaderValue("content-type").empty()) {
		return 0;
	}

	if (!status.empty()) {
		response.setStatusCode(HttpResponse::numberToStatusCode(Utils::toNumber(status)));
	} else {
		response.setStatusCode(http_status_code::FOUND);
	}
	response.setHeaderValue("location", location);
	response.setHeaderValue("content-type", cgi_response.getHeaderValue("content-type"));
	response.setBody(cgi_response.getBody());
	std::cerr << "withDoc Done" << std::endl;
	return 1;
}

int createClientRedirectResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	if (cgi_response.isHeaderKeyPresent("location") == false) {
		return 0;
	}
	std::string location = cgi_response.getHeaderValue("location");
	if (location.empty()) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	if (locationAnalysis(location) != 0) {
		return 0;
	}
	std::string status = cgi_response.getHeaderValue("status");
	if (!status.empty() && !isRedirectStatus(status)) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	http_status_code::STATUS_CODE response_status_code = http_status_code::FOUND;
	if (!status.empty()) {
		response_status_code = HttpResponse::numberToStatusCode(Utils::toNumber(status));
	}
	if (response_status_code != http_status_code::FOUND) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	response.setStatusCode(response_status_code);
	response.setHeaderValue("location", location);
	return 1;
}

int createLocalRedirectResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const ServerDirective& server_directive,
	const LocationDirective& location_directive) {
	if (cgi_response.isHeaderKeyPresent("location") == false) {
		return 0;
	}
	std::string location = cgi_response.getHeaderValue("location");
	if (location.empty()) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	std::string status = cgi_response.getHeaderValue("status");
	if (!status.empty() && !isRedirectStatus(status)) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
		return -1;
	}
	if (!status.empty()) {
		response.setStatusCode(HttpResponse::numberToStatusCode(Utils::toNumber(status)));
	} else {
		response.setStatusCode(http_status_code::SEE_OTHER);
	}
	std::string server_name = server_directive.getServerName();
	std::string server_port = server_directive.getPort();
	std::string local_pathquery = cgi_response.getHeaderValue("local-pathquery");
	std::string redirect_location =
		"http://" + server_name + ":" + server_port + location + local_pathquery;
	response.setHeaderValue("location", redirect_location);
	return 1;
}

bool isRedirectStatus(std::string status) {
	int number = -1;
	http_status_code::STATUS_CODE status_code = http_status_code::OK;
	std::istringstream iss(status);
	std::string tmp;
	while (iss >> tmp) {
		std::istringstream iss2(tmp);
		if (iss2 >> number) {
			break;
		}
	}
	status_code = HttpResponse::numberToStatusCode(number);
	if (status_code == http_status_code::FOUND || status_code == http_status_code::SEE_OTHER ||
		status_code == http_status_code::PERMANENT_REDIRECT) {
		return true;
	}
	return false;
}

int createDocumentResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	(void)location_directive;
	createStatusCode(cgi_response, response);
	createContentType(cgi_response, response);
	if (cgi_response.getContentLength() > 0) {
		std::string body = cgi_response.getBody();
		std::string new_body;
		for (std::size_t i = 0; i < body.size(); i++) {
			if (body[i] == '\n') {
				new_body += "\r\n";
			} else {
				new_body += body[i];
			}
		}
		response.setHeaderValue("content-length", Utils::toString(new_body.size()));
		response.setBody(new_body);
	}
	return 0;
}

void createStatusCode(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
	std::string status = cgi_response.getHeaderValue("status");
	http_status_code::STATUS_CODE status_code = http_status_code::OK;
	if (status.empty()) {
		response.setStatusCode(status_code);
	} else {
		int number = 200;
		// todo error check
		std::istringstream iss(status);
		std::string tmp;
		while (iss >> tmp) {
			std::istringstream iss2(tmp);
			if (iss2 >> number) {
				break;
			}
		}
		status_code = HttpResponse::numberToStatusCode(number);
		response.setStatusCode(status_code);
	}
}

void createContentType(cgi::CgiResponse const& cgi_response, HttpResponse& response) {
	std::string content_type = cgi_response.getHeaderValue("content-type");
	if (!content_type.empty()) {
		response.setHeaderValue("content-type", content_type);
	} else {
		response.setHeaderValue("content-type", "text/plain");
	}
}

} // namespace handle_cgi

} // namespace server
