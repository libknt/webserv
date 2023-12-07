#ifndef HANDLE_CGI_HPP
#define HANDLE_CGI_HPP

#include "cgi.hpp"
#include "cgi_response.hpp"
#include "client_session.hpp"
#include "handle_request.hpp"

#include <cctype>
#include <sstream>

namespace server {

namespace handle_cgi_response {

void handleCgiResponse(ClientSession& client_session);
void createResponseFromCgiResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive);
int createStatusCode(std::string const& status);
void createHeaderFiled(cgi::CgiResponse const& cgi_response, HttpResponse& response);
int locationAnalysis(std::string location, HttpResponse& response);

} // namespace handle_cgi_response

} // namespace server

#endif // HANDLE_CGI_HPP