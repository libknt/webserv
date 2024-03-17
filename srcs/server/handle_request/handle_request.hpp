#ifndef HANDLE_REQUEST_HPP
#define HANDLE_REQUEST_HPP

#include "client_session.hpp"
#include "configuration.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "utils.hpp"
#include <ctime>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

namespace server {

namespace handle_request {
void handleRequest(ClientSession& client_session);
void executeGet(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive);
void executePost(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive);
void executeDelete(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive);
void makeAutoIndex(HttpRequest const& request,
	HttpResponse& response,
	const LocationDirective& location_directive);
};

void createErrorResponse(HttpResponse& response,
	http_status_code::STATUS_CODE status_code,
	const LocationDirective& location_directive);

}

#endif