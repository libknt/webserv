#ifndef EXEC_RESPONSE_HPP
#define EXEC_RESPONSE_HPP
#include "http_request.hpp"
#include "http_response.hpp"

namespace server {
    HttpResponse    execResponse(HttpRequest const &request);
}
#endif