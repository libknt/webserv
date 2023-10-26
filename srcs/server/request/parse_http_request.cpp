#include "parse_http_request.hpp"
namespace server {

ParseHttpRequest::ParseHttpRequest() {}

ParseHttpRequest::~ParseHttpRequest() {}

ParseHttpRequest::ParseHttpRequest(ParseHttpRequest& other) {
	*this = other;
}

ParseHttpRequest& ParseHttpRequest::operator=(ParseHttpRequest& other) {
	if (this != &other) {
		http_request_map_ = other.http_request_map_;
		http_line_stream_ = other.http_line_stream_;
	}
	return (*this);
}
int ParseHttpRequest::handleBuffer(int socketfd, char* buf) {

	std::string buffer(buf);
	std::string::size_type index;

	http_line_stream_[socketfd] += buffer;
	std::map<int, HttpRequest>::iterator it = http_request_map_.find(socketfd);
	if (it->second.getHttpRequestStatus() == http_request_status::BODY)
		it->second.parseHttpRequest(http_line_stream_[socketfd]);
	else {
		while ((index = http_line_stream_[socketfd].find("\r\n")) != std::string::npos) {
			std::string line = http_line_stream_[socketfd].substr(0, index);
			http_line_stream_[socketfd] = http_line_stream_[socketfd].substr(index + 2);

			std::map<int, HttpRequest>::iterator it = http_request_map_.find(socketfd);
			if (it != http_request_map_.end()) {
				it->second.parseHttpRequest(line);
			}
			// TODO if error occured, you parseHttpRequest(line) must return -1. So handle it.
		}
	}
	if (it != http_request_map_.end()) {
		if (http_request_status::FINISHED == it->second.getHttpRequestStatus()) {
			return 1;
		}
	}
	return (0);
}

HttpRequest& ParseHttpRequest::get_http_request(int sd) {
	std::map<int, server::HttpRequest>::iterator it = http_request_map_.find(sd);
	return it->second;
}

void ParseHttpRequest::add_accept_client_info(int socketfd,
	sockaddr_in client_addr,
	sockaddr_in server_addr) {
	if (http_request_map_.find(socketfd) == http_request_map_.end()) {
		HttpRequest request(client_addr, server_addr);
		http_request_map_.insert(std::pair<int, HttpRequest>(socketfd, request));
	}
}

}
