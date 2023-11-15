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
			if (it == locations.end()) {
				std::cerr << "Location not found" << std::endl;
				//TODO 404 not found Error
				return (response);
			}
			LocationDirective location_directive = it->second;

			if (method == "GET") {
				response = executeGet(request, location_directive);
			} else if (method == "POST") {
				response = executePost(request, location_directive);
			} else if (method == "DELETE") {
				response = executeDelete(request, location_directive);
			} else {
				if (request.getStatus() == http_request_status::ERROR) {
					response.setStatusCode(BAD_REQUEST);
				}
				response = createErrorResponse(response, location_directive);
			}
		}
	}
	return response;
}

HttpResponse executeGet(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;

	std::string file_path = location_directive.getRoot() + "/" + location_directive.getIndex();
	std::cout << "Debug: " << file_path << std::endl;
	(void) request;
	return (response);
}

HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse executeDelete(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse createErrorResponse(HttpResponse& response, const LocationDirective& location_directive) {
	(void)location_directive;
	return (response);
}
};
};
