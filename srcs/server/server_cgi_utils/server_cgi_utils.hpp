#ifndef SERVER_CGI_UTILS_HPP
#define SERVER_CGI_UTILS_HPP

#include "http_request.hpp"
#include <arpa/inet.h>
#include <map>
#include <string>

namespace server_cgi_utils {

void createCgiMetaVariables(std::map<std::string, std::string>& meta_variables,
	server::HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address);

} // namespace server_cgi_utils

#endif