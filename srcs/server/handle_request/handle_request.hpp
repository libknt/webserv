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
HttpResponse executeGet(const HttpRequest& request, const Configuration& configuration);
HttpResponse executePost(const HttpRequest& request, const Configuration& configuration);
HttpResponse executeDelete(const HttpRequest& request, const Configuration& configuration);
HttpResponse executeError(const HttpRequest& request, const Configuration& configuration);
};

}

#endif