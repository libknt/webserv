#include "handle_cgi.hpp"

namespace server {

namespace handle_cgi_response {

void handleCgiResponse(ClientSession& client_session) {
	cgi::Cgi const& cgi = client_session.getCgi();
	HttpResponse& response = client_session.getResponse();
	const HttpRequest& request = client_session.getRequest();
	const ServerDirective& server_directive = client_session.getServerDirective();
	const LocationDirective& location_directive =
		server_directive.findLocation(request.getRequestPath());
	createResponseFromCgiResponse(cgi.getCgiOutput(), response, location_directive);
	response.setStatus(http_response_status::RESPONSE_SENDING);
	response.concatenateComponents();
}

void createResponseFromCgiResponse(std::string const& cgi_output,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	if (cgi_output.find("Location") != std::string::npos &&
		cgi_output.find("Status") == std::string::npos) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
	} else {
		std::string output(cgi_output);
		std::string header = output.substr(0, output.find("\n\n"));
		std::string body = output.substr(output.find("\n\n") + 2);
		toLowerCaseForHeaders(header);
		removeSpace(header);
		createHeaderFiled(header, response);
		if (header.find("location:") != std::string::npos) {
			if (locationAnalysis(header.substr(header.find("location:") + 9,
									 header.find("\n", header.find("location:") + 9) -
										 header.find("location:") - 9),
					response) < 0) {
				createErrorResponse(
					response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
				return;
			}
		}
		response.setHeaderValue("content-length", Utils::toString(body.size()));
		response.setBody(body);
	}
}

int locationAnalysis(std::string location, HttpResponse& response) {
	if (location.empty()) {
		return -1;
	}
	if (location.compare(0, 4, "http") == 0 || location.compare(0, 5, "https") == 0 ||
		location.compare(0, 3, "ftp") == 0 || location.compare(0, 6, "mailto") == 0 ||
		location.compare(0, 4, "file") == 0 || location.compare(0, 3, "tel") == 0 ||
		location.compare(0, 4, "data") == 0) {
		response.setHeaderValue("location", location);
		return 0;
	} else if (location.compare(0, 1, "/") == 0) {
		response.setHeaderValue("location", location);
		return 0;
	}
	return -1;
}

void removeSpace(std::string& str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == ':') {
			if (i > 0 && str[i - 1] == ' ') {
				str.erase(i - 1, 1);
				--i;
			}
			if (i < str.length() - 1 && str[i + 1] == ' ') {
				str.erase(i + 1, 1);
			}
		}
	}
}

void toLowerCaseForHeaders(std::string& str) {
	bool isKey = true;
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if (*it == ':') {
			isKey = false;
		}

		if (isKey && std::isalpha(*it)) {
			*it = std::tolower(*it);
		}

		if (*it == '\n') {
			isKey = true;
		}
	}
}

void createHeaderFiled(std::string& headerFiled, HttpResponse& response) {
	std::string::size_type position;
	std::string::size_type end;

	position = headerFiled.find("status:");
	if (position != std::string::npos) {
		end = headerFiled.find("\n", position);
		createStatusCode(headerFiled.substr(position + 7, end - position - 7), response);
	} else {
		response.setStatusCode(http_status_code::OK);
	}

	position = headerFiled.find("content-type:");
	if (position != std::string::npos) {
		end = headerFiled.find("\n", position);
		response.setHeaderValue(
			"content-type", headerFiled.substr(position + 13, end - position - 13));
	} else {
		response.setHeaderValue("content-type", "text/html");
	}
}

void createStatusCode(std::string line, HttpResponse& response) {
	std::string status_code;
	int number = 200;
	std::istringstream iss(line);
	std::string tmp;
	while (iss >> tmp) {
		std::istringstream iss2(tmp);
		if (iss2 >> number) {
			break;
		}
	}
	response.setStatusCode(HttpResponse::numberToStatusCode(number));
}

} // namespace handle_cgi

} // namespace server
