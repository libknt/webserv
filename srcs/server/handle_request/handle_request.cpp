#include "handle_request.hpp"

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration) {
	std::string method = request.getMethod();
	std::vector<ServerDirective> servers = configuration.getServers();
	HttpResponse response;

	for (size_t i = 0; i < servers.size(); i++) {
		ServerDirective server_directive = servers[i];
		if (server_directive.getPort() == request.getServerPort()) {
			std::map<std::string, LocationDirective> locations = server_directive.getLocations();
			std::map<std::string, LocationDirective>::iterator it = locations.find(request.getRequestPath());
			if (it != locations.end()) {
				std::cerr << "Location not found" << std::endl;
			}
			LocationDirective location_directive = it->second;

			if (method == "GET") {
				response = executeGet(request, servers[i]);
			} else if (method == "POST") {
				response = executePost(request, servers[i]);
			} else if (method == "DELETE") {
				response = executeDelete(request, servers[i]);
			} else {
				if (request.getStatus() == http_request_status::ERROR) {
					response.setStatusCode(BAD_REQUEST);
				}
				response = createErrorResponse(response, servers[i]);
			}
		}
	}
	return response;
}

HttpResponse executeGet(const HttpRequest& request, const ServerDirective& server_directive) {
	HttpResponse response;
	(void)request;
	(void)server_directive;
	return (response);
}

HttpResponse executePost(const HttpRequest& request, const ServerDirective& server_directive) {
	HttpResponse response;
	(void)request;
	(void)server_directive;
	return (response);
}

HttpResponse executeDelete(const HttpRequest& request, const ServerDirective& server_directive) {
	HttpResponse response;
	(void)request;
	(void)server_directive;
	return (response);
}

HttpResponse createErrorResponse(HttpResponse& response, const ServerDirective& server_directive) {
	(void)server_directive;
	return (response);
}
};
};
