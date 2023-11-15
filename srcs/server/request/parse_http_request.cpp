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

SERVER_STATUS ParseHttpRequest::handleBuffer(int socketfd, char* buf) {

	std::string buffer(buf);
	std::string::size_type index;
	http_line_stream_[socketfd] += buffer;

	std::map<int, HttpRequest>::iterator it = http_request_map_.find(socketfd);
	if (it == http_request_map_.end()) {
		std::cerr << "handleBuffer: find HttpRequest failed: " << __FILE__ << " : " << __LINE__
				  << std::endl;
		return PROCESSING_ERROR;
	}
	if (it->second.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH) {
		it->second.parseHttpRequest(http_line_stream_[socketfd]);
		http_line_stream_[socketfd].clear();
	}
	else {
		while ((index = http_line_stream_[socketfd].find("\r\n")) != std::string::npos) {
			std::string line = http_line_stream_[socketfd].substr(0, index);
			http_line_stream_[socketfd] = http_line_stream_[socketfd].substr(index + 2);

			it->second.parseHttpRequest(line);
			// TODO if error occured, you parseHttpRequest(line) must return -1. So handle it.
			if (it->second.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH) {
				it->second.parseHttpRequest(http_line_stream_[socketfd]);
				http_line_stream_[socketfd].clear();
			}
		}
	}

	if (http_request_status::FINISHED == it->second.getStatus()) {
		return PREPARING_RESPONSE;
	}
	return RECEIVING_REQUEST;
}

HttpRequest& ParseHttpRequest::getHttpRequest(int sd) {
	std::map<int, server::HttpRequest>::iterator it = http_request_map_.find(sd);
	return it->second;
}

int ParseHttpRequest::addAcceptClientInfo(int socketfd,
	sockaddr_in client_address,
	sockaddr_in server_address) {
	if (http_request_map_.find(socketfd) != http_request_map_.end()) {
		return -1;
	}
	HttpRequest request(client_address, server_address);
	http_request_map_.insert(std::pair<int, HttpRequest>(socketfd, request));
	return 0;
}

int ParseHttpRequest::httpRequestCleanup(int sd) {
	std::map<int, HttpRequest>::iterator it = http_request_map_.find(sd);
	if (it == http_request_map_.end()) {
		return -1;
	}
	it->second.cleanup();
	return 0;
}

int ParseHttpRequest::httpRequestErase(int sd) {
	http_request_map_.erase(sd);
	return 0;
}

void ParseHttpRequest::printAcceptedFds() {
	typedef std::map<int, HttpRequest>::iterator REQUEST_MAP_IT;
	for (REQUEST_MAP_IT it = http_request_map_.begin(); it != http_request_map_.end(); ++it) {
		std::cout << "fd : " << it->first << std::endl;
	}
}
}
