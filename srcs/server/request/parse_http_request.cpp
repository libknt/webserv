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
	HttpRequest request;
	std::string::size_type index;

	if (http_request_map_.find(socketfd) == http_request_map_.end()) {
		http_request_map_.insert(std::pair<int, HttpRequest>(socketfd, request));
	}
	http_line_stream_[socketfd] += buffer;
	if (http_request_map_[socketfd].getHttpBodyMessageType() == http_body_message_type::CONTENT_LENGTH)
		http_request_map_[socketfd].parseHttpRequest(http_line_stream_[socketfd]);
	else
	{
		while ((index = http_line_stream_[socketfd].find("\r\n")) != std::string::npos) {
			std::string line = http_line_stream_[socketfd].substr(0, index);
			http_line_stream_[socketfd] = http_line_stream_[socketfd].substr(index + 2);
			http_request_map_[socketfd].parseHttpRequest(line);
			// TODO if error occured, you parseHttpRequest(line) must return -1. So handle it.
		}
	}
	if (http_request_map_[socketfd].getHttpRequestStatus() == http_request_status::FINISHED)
		return(1);
	return (0);
}
}