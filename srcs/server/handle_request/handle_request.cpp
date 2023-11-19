#include "handle_request.hpp"

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration) {
	std::string method = request.getMethod();
	std::vector<ServerDirective> servers = configuration.getServers();
	HttpResponse response;

	// エラー: リソースが存在しない(), 許されていないmethod
	for (size_t i = 0; i < servers.size(); i++) {
		ServerDirective server_directive = servers[i];
		if (server_directive.getPort() == request.getServerPort()) {
			LocationDirective location_directive = server_directive.findLocation(request.getRequestPath());

			if (method == "GET" && location_directive.isAllowMethod(method)) {
				response = executeGet(request, location_directive);
			} else if (method == "POST" && location_directive.isAllowMethod(method)) {
				response = executePost(request, location_directive);
			} else if (method == "DELETE" && location_directive.isAllowMethod(method)) {
				response = executeDelete(request, location_directive);
			} else {
				response = createErrorResponse(METHOD_NOT_ALLOWED, location_directive);
			}
			break;
		}
	}
	return response;
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
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse createErrorResponse(const STATUS_CODE status_code,
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
			"<html><body><h1> createErrorResponse(): " + stringstream.str() + "</h1></body></html>";
	}
	response.setStatusCode(status_code);
	response.setHeaderValue("Content-Type", "text/html");
	response.setBody(body_content);
	return response;
}
};
};
