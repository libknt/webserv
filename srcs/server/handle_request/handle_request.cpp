#include "handle_request.hpp"

namespace server {

	namespace handle_request {
		HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration) {
			std::string method = request.getMethod();
			HttpResponse response;

			if (method == "GET") {
				// GETの処理
				response = executeGet(request, configuration);
			} else if (method == "POST") {
				// POSTの処理
				response = executePost(request, configuration);
			} else if (method == "DELETE") {
				// DELETEの処理
				response = executeDelete(request, configuration);
			} else {
				// Errorの処理
				response = executeError(request, configuration);
			}
			return response;
		}

		HttpResponse executeGet(const HttpRequest& request, const Configuration& configuration) {
			HttpResponse response;
			(void) request;
			(void) configuration;
			return (response);
		}

		HttpResponse executePost(const HttpRequest& request, const Configuration& configuration) {
			HttpResponse response;
			(void) request;
			(void) configuration;
			return (response);
		}

		HttpResponse executeDelete(const HttpRequest& request, const Configuration& configuration) {
			HttpResponse response;
			(void) request;
			(void) configuration;
			return (response);
		}

		HttpResponse executeError(const HttpRequest& request, const Configuration& configuration) {
			HttpResponse response;
			(void) request;
			(void) configuration;
			return (response);
		}
	};
};
