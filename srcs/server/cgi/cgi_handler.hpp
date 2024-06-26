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
	const ServerDirective& server_directive,
	const LocationDirective& location_directive);
int createDocumentResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive);
void createStatusCode(cgi::CgiResponse const& cgi_response, HttpResponse& response);
void createContentType(cgi::CgiResponse const& cgi_response, HttpResponse& response);
int createLocalRedirectResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const ServerDirective& server_directive,
	const LocationDirective& location_directive);
int createClientRedirectResponse(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive);
bool isRedirectStatus(std::string status);
int createClientRedirectResponseWithDocument(cgi::CgiResponse const& cgi_response,
	HttpResponse& response,
	const LocationDirective& location_directive);

} // namespace cgi_handler

} // namespace server

#endif // HANDLE_CGI_HPP