#include "handle_request.hpp"

namespace server {

namespace handle_request {
void handleRequest(ClientSession& client_session) {
	HttpRequest& request = client_session.getRequest();
	HttpResponse& response = client_session.getResponse();

	ServerDirective server_directive = client_session.getServerDirective();

	if (server_directive.getPort() == client_session.getServerPort()) {
		LocationDirective location_directive =
			server_directive.findLocation(request.getRequestPath());
		std::string const method = request.getMethod();

		if (method == "GET" && location_directive.isAllowMethod(method)) {
			response = executeGet(request, location_directive);
		} else if (method == "POST" && location_directive.isAllowMethod(method)) {
			response = executePost(request, location_directive);
		} else if (method == "DELETE" && location_directive.isAllowMethod(method)) {
			response = executeDelete(request, location_directive);
		} else {
			response =
				createErrorResponse(http_status_code::METHOD_NOT_ALLOWED, location_directive);
		}
	}
}

HttpResponse executeGet(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse executeDelete(const HttpRequest& request,
	const LocationDirective& location_directive) {
	struct stat file_status;
	std::string request_path = location_directive.getRoot() + request.getRequestPath();

	if (stat(request_path.c_str(), &file_status) == -1) {
		std::cerr << "DELETE Error: stat() failed" << std::endl;
		return createErrorResponse(http_status_code::NOT_FOUND, location_directive);
	}

	if (!(file_status.st_mode & S_IROTH) || !(file_status.st_mode & S_IWOTH)) {
		std::cerr << "DELETE Error: Permission denied" << std::endl;
		return createErrorResponse(http_status_code::BAD_REQUEST, location_directive);
	}

	if (remove(request_path.c_str()) != 0) {
		std::cerr << "DELETE Error: remove() falied" << std::endl;
		return createErrorResponse(http_status_code::BAD_REQUEST, location_directive);
	}

	HttpResponse response;
	response.setStatusCode(http_status_code::NO_CONTENT);
	return response;
}

HttpResponse createErrorResponse(const http_status_code::STATUS_CODE status_code,
	const LocationDirective& location_directive) {
	HttpResponse response;

	std::map<std::string, std::string> error_pages = location_directive.getErrorPages();

	std::stringstream stringstream;
	stringstream << status_code;
	std::string error_page_path = error_pages[stringstream.str()];

	std::ifstream file_stream(error_page_path.c_str());
	std::string line, body_content;
	if (file_stream.is_open()) {
		while (getline(file_stream, line)) {
			body_content += line + "\n";
		}
		file_stream.close();
	} else {
		body_content =
			"<html><body><h1> setErrorResponse(): " + stringstream.str() + "</h1></body></html>";
	}
	response.setStatusCode(status_code);
	response.setHeaderValue("Content-Type", "text/html");
	response.setHeaderValue("Content-Length", std::to_string(body_content.size()));
	response.setBody(body_content);
	return response;
}
};
};
