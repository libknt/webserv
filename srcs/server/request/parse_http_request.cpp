#include "parse_http_request.hpp"
namespace server {

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequestParser::HttpRequestParser(HttpRequestParser& other) {
	*this = other;
}

HttpRequestParser& HttpRequestParser::operator=(HttpRequestParser& other) {
	if (this != &other) {
		http_request_map_ = other.http_request_map_;
		http_line_stream_ = other.http_line_stream_;
	}
	return (*this);
}

int HttpRequestParser::handleBuffer(int sd, char const* buf) {

	std::string buffer(buf);
	std::string::size_type index;
	http_line_stream_[sd] += buffer;

	std::map<int, HttpRequest>::iterator it = http_request_map_.find(sd);
	if (it == http_request_map_.end()) {
		std::cerr << "map find() err: " << __FILE__ << " : " << __LINE__ << std::endl;
		exit(-1);
	}

	if (!(it->second.getStatus() == http_request_status::BODY) ||
		!(it->second.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH)) {
		while ((index = http_line_stream_[sd].find("\r\n")) != std::string::npos) {
			std::string line = http_line_stream_[sd].substr(0, index);
			http_line_stream_[sd] = http_line_stream_[sd].substr(index + 2);
			parseRequest(sd, line);
		}
	}

	if (it->second.getStatus() == http_request_status::BODY &&
		it->second.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH) {
		parseRequest(sd, http_line_stream_[sd]);
		http_line_stream_[sd].clear();
	}

	if (it->second.getStatus() == http_request_status::FINISHED) {
		return 1;
	}
	return (0);
}

HttpRequest const& HttpRequestParser::getRequest(int sd) const {
	std::map<int, server::HttpRequest>::const_iterator it = http_request_map_.find(sd);
	return it->second;
}

void HttpRequestParser::AcceptClientInfo(int sd,
	sockaddr_in client_address,
	sockaddr_in server_address) {
	if (http_request_map_.find(sd) == http_request_map_.end()) {
		HttpRequest request(client_address, server_address);
		http_request_map_.insert(std::pair<int, HttpRequest>(sd, request));
	}
}

int HttpRequestParser::parseRequest(int sd, std::string const &line) {
	std::map<int, HttpRequest>::iterator it = http_request_map_.find(sd);
	if (it == http_request_map_[sd].end()) {
		HttpRequest request
		request.setStatus(http_request_status::ERROR);
		http_request_map_.insert(std::make_pair(sd, request));
		return (-1);
	}
	switch (it->second.getStatus())
}

int HttpRequestParser::parseMethod(int sd);
int HttpRequestParser::parseHeader(int sd);
}
