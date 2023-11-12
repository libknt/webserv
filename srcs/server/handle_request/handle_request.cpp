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
				// GETの処理
				response = executeGet(request, servers[i]);
			} else if (method == "POST") {
				// POSTの処理
				response = executePost(request, servers[i]);
			} else if (method == "DELETE") {
				// DELETEの処理
				response = executeDelete(request, servers[i]);
			} else {
				// Errorの処理
				response = executeError(request, servers[i]);
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

HttpResponse executeError(const HttpRequest& request, const ServerDirective& server_directive) {
	HttpResponse response;
	(void)request;
	(void)server_directive;
	return (response);
}
};
};