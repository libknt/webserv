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

int HttpRequestParser::handleBuffer(int sd, char *buf) {

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

void HttpRequestParser::addAcceptClientInfo(int sd,
	sockaddr_in client_address,
	sockaddr_in server_address) {
	if (http_request_map_.find(sd) == http_request_map_.end()) {
		HttpRequest request(client_address, server_address);
		http_request_map_.insert(std::pair<int, HttpRequest>(sd, request));
	}
}

int HttpRequestParser::parseRequest(int sd, std::string &line) {
	std::map<int, HttpRequest>::iterator it = http_request_map_.find(sd);
	if (it == http_request_map_.end()) {
		//TODO somethig happened when accpect;
		return (-1);
	}
	switch (it->second.getStatus()) {
		case http_request_status::METHOD:
			parseStartLine(it->second, line);
			break;
		case http_request_status::HEADER:
			//parseHeader(it->second, line);
			break;
		case http_request_status::BODY:
			std::cout << "parseBODY" << std::endl;
			break;
		case http_request_status::FINISHED:
			std::cout << "FINISHED" << std::endl;
			break;
		case http_request_status::ERROR:
			std::cout << "Error" << std::endl;
			break;
		case http_request_status::UNDEFINED:
			std::cout << "UNDEFINED" << std::endl;
			break;
	}
	return (0);
}

int HttpRequestParser::parseStartLine(HttpRequest &request, std::string &line) {

	size_t index = 0;
	parse_start_line::PARSE_START_LINE status = parse_start_line::METHOD;
		
	for (size_t i = 0; i < line.size(); i++) {
		if (line[i] == ' ') {
			switch (status) {
				case parse_start_line::METHOD:
					if (request.setMethod(line.substr(index, i - index)) < 0) {
						return (-1);
					}
					index = i + 1;
					status = parse_start_line::REQUEST_PATH;
					break;
				case parse_start_line::REQUEST_PATH:
					if (request.setRequestPath(line.substr(index, i - index)) < 0) {
						return (-1);
					}
					index = i + 1;
					status = parse_start_line::VERSION;
					break;
				default:
					request.setStatus(http_request_status::ERROR);
					break;
			}
		}
	}
	if (status != parse_start_line::VERSION || request.setVersion(line.substr(index, line.size() - index))) {
		request.setStatus(http_request_status::ERROR);
		return (-1);
	}
	request.setStatus(http_request_status::HEADER);
	return (0);
}

//int HttpRequestParser::parseHeader(HttpRequest &request, std::string &line) {
//	
//	for (size_t i = 0; i < line.size(); i++) {
//		if (IS_TCHAR(line[i]))
//	}
//}

}
