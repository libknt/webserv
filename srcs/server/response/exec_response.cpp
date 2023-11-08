#include "exec_request.hpp"

namespace server {
    HttpResponse    execResponse(HttpRequest const &request, Configuration const &configuration) {
        
        std::string method = request.getMethod();
        if (method == "GET") {
            return (execGET(request, configuration));
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
    HttpResponse    execGET(HttpRequest const &request, Configuration const &configuration) {
        HttpResponse response;
        std::vector<ServerDirective> servers = configuration.getServers();
        for (size_t i = 0; i < servers.size(); i++) {
            if (ntohs(request.getServerAddress().sin_port) == std::atoi(servers[i].getPort().c_str()))
                std::cout << "Correct Port is " << std::atoi(servers[i].getPort().c_str()) << std::endl;
            else
                std::cout << "Wrong Port" << std::endl;
        }
        return (response);
    }
}