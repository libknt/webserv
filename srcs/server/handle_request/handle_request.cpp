#include "handle_request.hpp"

namespace server {

namespace handle_request {
void handleRequest(ClientSession& client_session) {
	HttpRequest const& request = client_session.getRequest();
	HttpResponse& response = client_session.getResponse();

	const ServerDirective& server_directive = client_session.getServerDirective();
	const LocationDirective& location_directive =
		server_directive.findLocation(request.getUriPath());

	const std::string return_directive = location_directive.getReturn();
	if (!return_directive.empty()) {
		response.setStatusCode(http_status_code::FOUND);
		response.setHeaderValue("Location", return_directive);
		response.concatenateComponents();
		client_session.setStatus(SENDING_RESPONSE);
		return;
	}
	std::cout << request << std::endl;
	if (request.getStatus() == http_request_status::ERROR) {
		createErrorResponse(response, request.getHttpStatusCode(), location_directive);
		response.concatenateComponents();
		client_session.setStatus(SENDING_RESPONSE);
		return;
	}
	std::string const method = request.getMethod();
	if (method == "GET" && location_directive.isAllowMethod(method)) {
		executeGet(request, response, location_directive);
	} else if (method == "POST" && location_directive.isAllowMethod(method)) {
		executePost(request, response, location_directive);
	} else if (method == "DELETE" && location_directive.isAllowMethod(method)) {
		executeDelete(request, response, location_directive);
	} else {
		createErrorResponse(response, http_status_code::METHOD_NOT_ALLOWED, location_directive);
	}
	response.concatenateComponents();
	client_session.setStatus(SENDING_RESPONSE);
}

void executeGet(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	struct stat location_stat_info;
	struct stat request_stat_info;
	std::string file_path = location_directive.getRoot() + request.getUri();
	std::string location_path =
		location_directive.getRoot() + "/" + location_directive.getLocationPath();

	if (stat(file_path.c_str(), &request_stat_info) != 0 ||
		stat(location_path.c_str(), &location_stat_info) != 0) {
		return (createErrorResponse(response, http_status_code::NOT_FOUND, location_directive));
	}
	if (S_ISREG(request_stat_info.st_mode)) {
		std::ifstream file_stream(file_path.c_str());
		std::stringstream body_stream;
		if (file_stream.is_open()) {
			response.setStatusCode(http_status_code::OK);
			body_stream << file_stream.rdbuf();
			response.setBody(body_stream.str());
			response.setHeaderValue("Content-Length", Utils::toString(response.getBodyLength()));
			response.setHeaderValue("Content-Type", response.getFileContentType(file_path));
			response.setStatus(http_response_status::RESPONSE_SENDING);
			return;
		}
	} else if (S_ISDIR(request_stat_info.st_mode)) {
		if (location_stat_info.st_ino == request_stat_info.st_ino) {
			std::string default_file_path = location_path + "/" + location_directive.getIndex();
			std::ifstream default_file_stream(default_file_path.c_str());
			if (default_file_stream.is_open()) {
				std::stringstream body_stream;
				response.setStatusCode(http_status_code::OK);
				body_stream << default_file_stream.rdbuf();
				response.setBody(body_stream.str());
				response.setHeaderValue(
					"Content-Length", Utils::toString(response.getBodyLength()));
				response.setHeaderValue(
					"Content-Type", response.getFileContentType(default_file_path));
				response.setStatus(http_response_status::RESPONSE_SENDING);
				return;
			}
		}
		if (location_directive.getAutoindex()) {
			return makeAutoIndex(request, response, location_directive);
		}
	}
	return (createErrorResponse(response, http_status_code::NOT_FOUND, location_directive));
}

void executePost(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	struct stat file_info;
	std::string file_path = location_directive.getRoot() + "/" + request.getUriPath();
	if (stat(file_path.c_str(), &file_info) != 0) {
		return (createErrorResponse(response, http_status_code::NOT_FOUND, location_directive));
	}
	if (S_ISREG(file_info.st_mode)) {
		std::ofstream file_stream(file_path.c_str());
		if (file_stream.is_open()) {
			file_stream << request.getBody() << std::endl;
			response.setStatusCode(http_status_code::CREATED);
			response.setHeaderValue("Location", request.getUriPath());
			response.setHeaderValue("Content-Length", Utils::toString(request.getBody().size()));
			response.setBody(request.getBody());
			response.setStatus(http_response_status::RESPONSE_SENDING);
			return;
		}
	} else if (S_ISDIR(file_info.st_mode)) {
		std::time_t time_val = std::time(NULL);
		std::ofstream file_stream(std::string(file_path + Utils::toString(time_val)).c_str());
		if (file_stream.is_open()) {
			file_stream << request.getBody() << std::endl;
			response.setStatusCode(http_status_code::CREATED);
			response.setHeaderValue("Location", request.getUriPath() + Utils::toString(time_val));
			response.setHeaderValue("Content-Length", Utils::toString(request.getBody().size()));
			response.setBody(request.getBody());
			response.setStatus(http_response_status::RESPONSE_SENDING);
			return;
		}
	}
	return (createErrorResponse(response, http_status_code::NOT_FOUND, location_directive));
}

void executeDelete(const HttpRequest& request,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	struct stat file_info;
	std::string uri = location_directive.getRoot() + request.getUriPath();

	if (stat(uri.c_str(), &file_info) != 0) {
		std::cerr << "DELETE Error: stat() failed" << std::endl;
		return createErrorResponse(response, http_status_code::NOT_FOUND, location_directive);
	}

	if (!(file_info.st_mode & S_IROTH) || !(file_info.st_mode & S_IWOTH)) {
		std::cerr << "DELETE Error: Permission denied" << std::endl;
		return createErrorResponse(response, http_status_code::BAD_REQUEST, location_directive);
	}

	if (std::remove(uri.c_str()) != 0) {
		std::cerr << "DELETE Error: remove() falied" << std::endl;
		return createErrorResponse(response, http_status_code::BAD_REQUEST, location_directive);
	}
	response.setStatusCode(http_status_code::NO_CONTENT);
	response.setStatus(http_response_status::RESPONSE_SENDING);
}

void makeAutoIndex(HttpRequest const& request,
	HttpResponse& response,
	const LocationDirective& location_directive) {
	std::string const request_path = request.getUri();
	std::string const root = location_directive.getRoot() + request_path;
	DIR* dir = opendir(root.c_str());
	if (!dir) {
		return (createErrorResponse(response, http_status_code::NOT_FOUND, location_directive));
	} else {
		std::string body;
		struct dirent* ent;
		while ((ent = readdir(dir))) {
			if (std::string(ent->d_name) == ".")
				continue;
			else if (std::string(ent->d_name) == "..")
				body = ("<a href=\"" + std::string(ent->d_name) + "/\">" +
						   std::string(ent->d_name) + "</a>\n") +
					   body;
			else if (ent->d_type == DT_DIR)
				body += ("<a href=\"" + request.getUri() + (request_path == "/" ? "" : "/") +
						 std::string(ent->d_name) + "/\">" + std::string(ent->d_name) + "</a>\n");
			else
				body += ("<a href=\"" + request.getUri() + (request_path == "/" ? "" : "/") +
						 std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>\n");
		}
		body = "<html>\n<head>\n<title>Index of</title>\n</head>\n<body>\n<h1>Index of" +
			   location_directive.getLocationPath() + "<h1>\n<hr>\n<pre>\n" + body;
		body += ("</pre>\n</hr>\n</body>\n</html>\n");
		response.setStatusCode(http_status_code::OK);
		response.setHeaderValue("Content-Length", Utils::toString(body.size()));
		response.setHeaderValue("Content-Type", "text/html");
		response.setBody(body);
	}
	closedir(dir);
	response.setStatus(http_response_status::RESPONSE_SENDING);
}
};

void createErrorResponse(HttpResponse& response,
	http_status_code::STATUS_CODE status_code,
	const LocationDirective& location_directive) {
	std::string file_path;

	if (location_directive.findErrorPagePath(status_code) != "")
		file_path =
			location_directive.getRoot() + "/" + location_directive.findErrorPagePath(status_code);
	else
		file_path = location_directive.getRoot() + "/" + location_directive.getDefaultErrorPage();
	std::ifstream file_stream(file_path.c_str());
	std::string line, body_content;
	if (file_stream.is_open()) {
		while (getline(file_stream, line)) {
			body_content += line + "\n";
		}
	} else {
		body_content = "<html><body><h1> setErrorResponse(): " + Utils::toString(status_code) +
					   "</h1></body></html>";
	}
	response.setStatusCode(status_code);
	response.setHeaderValue("Content-Type", "text/html");
	response.setHeaderValue("Content-Length", Utils::toString(body_content.size()));
	response.setBody(body_content);
	response.setStatus(http_response_status::RESPONSE_SENDING);
}
};
