#include "handle_request.hpp"

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration) {
	std::string method = request.getMethod();
	std::vector<ServerDirective> servers = configuration.getServers();
	HttpResponse response;

	for (size_t i = 0; i < servers.size(); i++) {
		if (servers[i].getPort() == request.getServerPort())
		{
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
	const char* request_path = request.getRequestPath().c_str();
	struct stat file_status;

	if (stat(request_path, &file_status) == -1) {
		std::cerr << "DELETE Error: stat() failed" << std::endl;
		return executeError(request, server_directive);
	}

	if (!(file_status.st_mode & S_IROTH) || !(file_status.st_mode & S_IWOTH)) {
		std::cerr << "DELETE Error: Permission denied" << std::endl;
		return executeError(request, server_directive);
	}

	if (remove(request.getRequestPath().c_str()) != 0) {
		std::cerr << "DELETE Error: remove() falied" << std::endl;
		return executeError(request, server_directive);
	}

	return response;
}

HttpResponse createErrorResponse(HttpResponse& response, const ServerDirective& server_directive) {
	(void)server_directive;
	return (response);
}
};
};
