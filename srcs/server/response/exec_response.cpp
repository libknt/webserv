#include "http_request.hpp"
#include "http_response.hpp"

namespace server {
    HttpResponse    execResponse(HttpRequest const &request) {

        std::string method = request.getMethod();
        if (method == "GET") {

        }
        else if (method == "POST") {

        }
        else if (method == "DELETE") {

        }
        else {

        }
        HttpResponse response;
        return (response);
    }
}