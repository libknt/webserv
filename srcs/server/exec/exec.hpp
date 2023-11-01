#ifndef EXEC_HPP
#define EXEC_HPP
#include "http_request.hpp"
#include "http_response.hpp"
#include <unistd.h>
#include <sys/stat.h>

namespace server {

    HttpResponse    executeRequest(HttpRequest const &request);
    HttpResponse    executeGet(HttpRequest const &request);
    HttpResponse    executeError(HttpRequest const &request);
}
#endif