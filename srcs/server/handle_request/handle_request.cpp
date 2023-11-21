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
			// TODO: locationがない場合でもデフォルトの設定を適用し、エラーがでないようにする
			std::map<std::string, LocationDirective> locations = server_directive.getLocations();
			LocationDirective location_directive = locations[request.getRequestPath()];

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
	struct stat stat_info;

	std::string file_path = location_directive.getRoot() + "/" + location_directive.getIndex();
	if (stat(file_path.c_str(), &stat_info) != 0) {
		if (location_directive.getAutoindex() == "on") {
			response = makeAutoIndex(location_directive);
			//TODO make auto index
			
		}
		else {
			executeDelete(request, location_directive);
		}
	}
	else {
		std::ifstream file_stream(file_path);
		std::string body;
		response.setStatusCode(OK);
		response.setHeaderValue("Content-Length", std::to_string(stat_info.st_size));
		std::getline(file_stream, body, static_cast<char>(EOF));
		response.setBody(body);
	}
	(void) request;
	return (response);
}

HttpResponse executePost(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse executeDelete(const HttpRequest& request, const LocationDirective& location_directive) {
	HttpResponse response;
	(void)request;
	(void)location_directive;
	return (response);
}

HttpResponse createErrorResponse(const STATUS_CODE status_code, const LocationDirective& location_directive) {
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
        body_content = "<html><body><h1> testestestest create Error Response: " + stringstream.str() + "</h1></body></html>";
    }
	response.setStatusCode(status_code);
	response.setHeaderValue("Content-Type", "text/html");
	response.setBody(body_content);
	return response;
}

HttpResponse makeAutoIndex(const LocationDirective& location_directive) {
	HttpResponse response;
	std::string const root = location_directive.getRoot();
	DIR *dir = opendir(root.c_str());
	if (!dir) {
		std::cerr << "Error" << std::endl;
	} else {
		std::string body = "<html>\n<head>\n<title>Index of</title>\n</head>\n";
		body += "<body>\n<h1>Index of<h1>\n";
		body += "<hr>\n<pre>\n";
		struct dirent *ent;
		while ((ent = readdir(dir))) {
			body+= ("<a href=" + std::string(ent->d_name) + ">" + std::string(ent->d_name) + "</a>\n");
		}
		body+= ("</pre>\n</hr>\n</body>\n</html>\n");
		response.setHeaderValue("Content-Length", std::to_string(body.size()));
		response.setBody(body);
	}
	closedir(dir);
	return (response);
}
};
};
