#include "handle_request.hpp"


HttpResponse& handleRequest(const HttpRequest& request, const Configuration& configuration) {
	std::string method = request.getMethod();
	HttpResponse response;

	if (method == "GET") {
		// GETの処理
	} else if (method == "POST") {
		// POSTの処理
	} else if (method == "DELETE") {
		// DELETEの処理
	} else {
		// Errorの処理
	}
	return response;
}

HttpResponse& executeGet(const HttpRequest& request, const Configuration& configuration) {

}

HttpResponse& executePost(const HttpRequest& request, const Configuration& configuration) {

}

HttpResponse& executeDelete(const HttpRequest& request, const Configuration& configuration) {

}