#include "handle_cgi.hpp"

namespace server {

namespace handle_request {

void handleCgiResponse(ClientSession& client_session) {
	Cgi const& cgi = client_session.getCgi();
	HttpResponse& response = client_session.getResponse();
	const HttpRequest& request = client_session.getRequest();
	const ServerDirective& server_directive = client_session.getServerDirective();
	const LocationDirective& location_directive =
		server_directive.findLocation(request.getRequestPath());
	// std::cout << "\033[31m" << response << "\033[0m" << std::endl;
	createResponseFromCgiResponse(cgi.getCgiOutput(), response, location_directive);
	// std::cout << "\033[32m" << response << "\033[0m" << std::endl;
	response.setStatus(http_response_status::RESPONSE_SENDING);
	response.concatenateComponents();
}

void createResponseFromCgiResponse(std::string const& cgi_output,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	if (cgi_output.find("Location: ") != std::string::npos &&
		cgi_output.find("Status: ") == std::string::npos) {
		createErrorResponse(response, http_status_code::INTERNAL_SERVER_ERROR, location_directive);
	} else {
		std::string output(cgi_output);
		std::string header = output.substr(0, output.find("\n\n"));
		std::string body = output.substr(output.find("\n\n") + 2);
		toLowerCaseForHeaders(header);
		createHeaderFiled(header, response);
		if (header.find("location: ") != std::string::npos) {
			if (locationAnalysis(output.substr(output.find("Location: ") + 10,
									 output.find("\n", output.find("Location: ") + 10) -
										 output.find("Location: ") - 10),
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
	//     http：ハイパーテキスト転送プロトコル。
	// https：安全なハイパーテキスト転送プロトコル。
	// ftp：ファイル転送プロトコル。
	// mailto：電子メールアドレス。
	// file：ローカルまたはネットワークファイルシステム上のファイル。
	// tel：電話番号。
	// data
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

	position = headerFiled.find("status: ");
	if (position != std::string::npos) {
		end = headerFiled.find("\n", position);
		createStatusCode(headerFiled.substr(position + 8, end - position - 8), response);
	} else {
		response.setStatusCode(http_status_code::OK);
	}

	position = headerFiled.find("content-type: ");
	if (position != std::string::npos) {
		end = headerFiled.find("\n", position);
		response.setHeaderValue(
			"content-type", headerFiled.substr(position + 14, end - position - 14));
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
