#ifndef HANDLE_REQUEST_HPP
# define HANDLE_REQUEST_HPP

# include <iostream>
# include <string>
# include "http_request.hpp"
# include "http_response.hpp"

namespace handle_request {
	HttpResponse& handleRequest(const HttpRequest& request, const Configuration& configuration);
	HttpResponse& executeGet(const HttpRequest& request, const Configuration& configuration);
	HttpResponse& executePost(const HttpRequest& request, const Configuration& configuration);
	HttpResponse& executeDelete(const HttpRequest& request, const Configuration& configuration);
};

#endif 