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
				return executeGet(request, servers[i]);
			} else if (method == "POST") {
				// POSTの処理
				return executePost(request, servers[i]);
			} else if (method == "DELETE") {
				// DELETEの処理
				return executeDelete(request, servers[i]);
			} else {
				// Errorの処理
				return executeError(request, configuration);
			}
		}
	}
	return executeError(request, configuration);
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

HttpResponse executeError(const HttpRequest& request, const Configuration& configuration) {
	HttpResponse response;
	(void)request;
	(void)configuration;
	return (response);
}
};
};
