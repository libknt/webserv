#include "handle_request.hpp"
#include <sys/stat.h>

namespace server {

namespace handle_request {
HttpResponse handleRequest(const HttpRequest& request, const Configuration& configuration) {
	std::string method = request.getMethod();
	std::vector<ServerDirective> servers = configuration.getServers();
	HttpResponse response;

	// エラー: リソースが存在しない(), 許されていないmethod
	for (size_t i = 0; i < servers.size(); i++) {
		ServerDirective server_directive = servers[i];
		if (server_directive.getPort() == request.getServerPort()) {
			LocationDirective location_directive =
				server_directive.findLocation(request.getRequestPath());

			if (method == "GET" && location_directive.isAllowMethod(method)) {
				response = executeGet(request, location_directive);
			} else if (method == "POST" && location_directive.isAllowMethod(method)) {
				response = executePost(request, location_directive);
			} else if (method == "DELETE" && location_directive.isAllowMethod(method)) {
				response = executeDelete(request, location_directive);
			} else {
				response = createErrorResponse(METHOD_NOT_ALLOWED, location_directive);
			}
			break;
		}
	}
	return response;
}

HttpResponse executeGet(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	struct stat location_stat_info; 
	struct stat request_stat_info;
	std::string file_path = location_directive.getRoot() + request.getRequestPath();
	std::string	location_path = location_directive.getRoot() + location_directive.getLocationPath();

	if (stat(file_path.c_str(), &request_stat_info) != 0 || stat(location_path.c_str(), &location_stat_info) != 0) {
		return (createErrorResponse(NOT_FOUND, location_directive));
	}
	else if (S_ISREG(request_stat_info.st_mode)){
		std::ifstream file_stream(file_path);
		std::string body;
		if (file_stream.is_open()) {
			response.setStatusCode(OK);
			std::getline(file_stream, body, static_cast<char>(EOF));
			response.setHeaderValue("Content-Length", std::to_string(body.size()));
			response.setBody(body);
			return (response);
		}
		else {
			return (createErrorResponse(Internal_Server_Error, location_directive));
		}
	}
	else if (S_ISDIR(request_stat_info.st_mode)) {

		if (location_stat_info.st_ino == request_stat_info.st_ino) {
			std::ifstream default_file_stream(location_directive.getRoot() + "/"+ location_directive.getIndex());
			std::string body;
			if (default_file_stream.is_open()) {
				response.setStatusCode(OK);
				std::getline(default_file_stream, body, static_cast<char>(EOF));
				response.setHeaderValue("Content-Length", std::to_string(body.size()));
				response.setBody(body);
				return (response);
			}
		}
		if (location_directive.getAutoindex() == "on")
			return (makeAutoIndex(request, location_directive));
		else
			return (createErrorResponse(NOT_FOUND, location_directive));
	}
	return (createErrorResponse(FORBIDDEN, location_directive));
}

HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse executeDelete(const HttpRequest& request,
	const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse createErrorResponse(const STATUS_CODE status_code,
	const LocationDirective& location_directive) {
	HttpResponse response;

	std::map<std::string, std::string> error_pages = location_directive.getErrorPages();

	std::stringstream stringstream;
	stringstream << status_code;
	std::string error_page_path = error_pages[stringstream.str()];

	std::ifstream file_stream(error_page_path.c_str());
	std::string line, body_content;
	if (file_stream.is_open()) {
		while (getline(file_stream, line)) {
			body_content += line + "\n";
		}
		file_stream.close();
	} else {
		body_content =
			"<html><body><h1> createErrorResponse(): " + stringstream.str() + "</h1></body></html>";
	}
	response.setStatusCode(status_code);
	response.setHeaderValue("Content-Type", "text/html");
	response.setHeaderValue("Content-Length", std::to_string(body_content.size()));
	response.setBody(body_content);
	return response;
}

HttpResponse makeAutoIndex(HttpRequest const &request, const LocationDirective& location_directive) {
	HttpResponse response;
	std::string const root = location_directive.getRoot() + request.getRequestPath();
	DIR *dir = opendir(root.c_str());
	if (!dir) {
		return (createErrorResponse(NOT_FOUND, location_directive));
	} else {
		std::string body;
		struct dirent *ent;
		while ((ent = readdir(dir))) {
			if (std::string(ent->d_name) == ".")
				continue;
			else if (std::string(ent->d_name) == "..")
				body = ("<a href=\"" + std::string(ent->d_name) + "/\">" + std::string(ent->d_name) + "</a>\n") + body;
			else if (ent->d_type == DT_DIR)
				body+= ("<a href=\"" + std::string(ent->d_name) + "/\">" + std::string(ent->d_name) + "</a>\n");
			else
				body+= ("<a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>\n");
			}
		body = "<html>\n<head>\n<title>Index of</title>\n</head>\n<body>\n<h1>Index of" + location_directive.getLocationPath() + "<h1>\n<hr>\n<pre>\n" + body;
		body+= ("</pre>\n</hr>\n</body>\n</html>\n");
		response.setStatusCode(OK);
		response.setHeaderValue("Content-Length", std::to_string(body.size()));
		response.setBody(body);
	}
	closedir(dir);
	return (response);
}
};
};
