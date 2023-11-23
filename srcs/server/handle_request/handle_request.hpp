#ifndef HANDLE_REQUEST_HPP
#define HANDLE_REQUEST_HPP

#include "configuration.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration);
HttpResponse executeGet(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse executeDelete(const HttpRequest& request, const LocationDirective& location_directive);
HttpResponse setErrorResponse(const STATUS_CODE status_code,
	const LocationDirective& location_directive);
};

}

#endif