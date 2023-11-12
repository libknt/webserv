#ifndef HANDLE_REQUEST_HPP
#define HANDLE_REQUEST_HPP

#include "configuration.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include <iostream>
#include <string>

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration);
HttpResponse executeGet(const HttpRequest& request, const ServerDirective& server_directive);
HttpResponse executePost(const HttpRequest& request, const ServerDirective& server_directive);
HttpResponse executeDelete(const HttpRequest& request, const ServerDirective& server_directive);
HttpResponse createErrorResponse(HttpResponse& response, const ServerDirective& server_directive);
};

}

#endif