#include "parse_http_request.hpp"

int ParseHttpRequest::parse_http_request(int socketfd, char* buf) {

	std::string buffer(buf);
	HttpRequest request;
	std::string::size_type index;

	if (http_request_map_.find(socketfd) == http_request_map_.end()) {
		http_request_map_[socketfd] = request;
	}
	http_line_stream_[socketfd] += buffer;
	while ((index = http_line_stream_[socketfd].find("\r\n")) != std::string::npos) {
		std::string line = http_line_stream_[socketfd].substr(0, index);
		http_line_stream_[socketfd] = http_line_stream_[socketfd].substr(index + 2);
		http_request_map_[socketfd].parseHttpRequest(line);
		// if error occured, you parseHttpRequest(line) must return -1. So handle it.
	}
	return (0);
}
