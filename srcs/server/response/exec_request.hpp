#ifndef EXEC_RESPONSE_HPP
#define EXEC_RESPONSE_HPP
#include "http_request.hpp"
#include "http_response.hpp"
#include "configuration.hpp"

namespace server {
    HttpResponse    execResponse(HttpRequest const &request, Configuration const &configuration);
    HttpResponse    execGET(HttpRequest const &request, Configuration const &configuration);
}
#endif