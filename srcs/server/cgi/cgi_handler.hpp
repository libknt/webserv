#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "cgi_request.hpp"
#include "cgi_response.hpp"
#include "client_session.hpp"
#include "handle_request.hpp"

#include <cctype>
#include <sstream>

namespace server {

namespace cgi_handler {

void handleCgiResponse(ClientSession& client_session);
void handleCgiProcess(ClientSession& client_session);
void createResponseFromCgiResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive);
int createStatusCode(std::string const& status);
void createHeaderFiled(cgi::CgiResponse const& cgi_response, HttpResponse& response);
int locationAnalysis(std::string location, HttpResponse& response);

} // namespace cgi_handler

} // namespace server

#endif // HANDLE_CGI_HPP