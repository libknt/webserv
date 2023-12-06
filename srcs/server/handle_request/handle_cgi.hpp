#ifndef HANDLE_CGI_HPP
#define HANDLE_CGI_HPP

#include "cgi.hpp"
#include "client_session.hpp"
#include "handle_request.hpp"

#include <cctype>
#include <sstream>

namespace server {

namespace handle_cgi_response {

void handleCgiResponse(ClientSession& client_session);
void createResponseFromCgiResponse(std::string const& cgi_output,
	HttpResponse& response,
	const LocationDirective& location_directive);
void createStatusCode(std::string cgi_output, HttpResponse& response);
void createHeaderFiled(std::string& headerFiled, HttpResponse& response);
void removeSpace(std::string& str);
void toLowerCaseForHeaders(std::string& str);
int locationAnalysis(std::string location, HttpResponse& response);

} // namespace handle_cgi_response

} // namespace server

#endif // HANDLE_CGI_HPP