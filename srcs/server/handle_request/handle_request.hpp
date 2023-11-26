#ifndef HANDLE_REQUEST_HPP
#define HANDLE_REQUEST_HPP

#include "client_session.hpp"
#include "configuration.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

namespace server {

namespace handle_request {
void handleRequest(ClientSession const& client_session);
HttpResponse executeGet(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse executeDelete(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse createErrorResponse(const http_status_code::STATUS_CODE status_code,
	const LocationDirective& location_directive);
};

}

#endif