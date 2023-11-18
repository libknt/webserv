#include "http_request_parser.hpp"
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

int HttpRequestParser::handleBuffer(int sd, char* buf) {

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

int HttpRequestParser::parseRequest(int sd, std::string const& line) {
	std::map<int, HttpRequest>::iterator it = http_request_map_.find(sd);
	if (it == http_request_map_.end()) {
		// TODO somethig happened when accpect;
		return (-1);
	}
	switch (it->second.getStatus()) {
		case http_request_status::METHOD:
			parseStartLine(it->second, line);
			break;
		case http_request_status::HEADER:
			parseHeader(it->second, line);
			break;
		case http_request_status::BODY:
			parseBody(it->second, line);
			break;
		default:
			it->second.setStatus(http_request_status::ERROR);
			break;
	}
	if (it->second.getStatus() == http_request_status::ERROR) {
		return (-1);
	}
	return (0);
}

int HttpRequestParser::parseStartLine(HttpRequest& request, std::string const& line) {

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
	if (status != parse_start_line::VERSION ||
		request.setVersion(line.substr(index, line.size() - index))) {
		request.setStatus(http_request_status::ERROR);
		return (-1);
	}
	request.setStatus(http_request_status::HEADER);
	return (0);
}

int HttpRequestParser::parseHeader(HttpRequest& request, std::string const& line) {

	std::string key;
	std::string value;
	std::string::size_type index = line.find(":");
	if (line.size() == 0) {
		return (checkHeaderValue(request));
	}
	if (index == std::string::npos) {
		request.setStatus(http_request_status::ERROR);
		return (-1);
	}
	key = line.substr(0, index);
	if (index + 1 < line.size()) {
		index++;
		while (line[index] == ' ')
			index++;
		value = line.substr(index);
	}
	request.setHeaderValue(key, value);
	return (0);
}

int HttpRequestParser::parseBody(HttpRequest& request, std::string const& line) {
	switch (request.getBodyMessageType()) {
		case http_body_message_type::CHUNK_ENCODING:
			return (parseChunkedBody(request, line));
		case http_body_message_type::CONTENT_LENGTH:
			return (parseContentLengthBody(request, line));
		default:
			request.setStatus(http_request_status::ERROR);
			request.setErrorStatus(http_error_status::BAD_REQUEST);
			return (-1);
	}
}

int HttpRequestParser::parseContentLengthBody(HttpRequest& request, std::string const& line) {
	request.addBody(line);
	if (request.getBodySize() == request.getContentLength())
		request.setStatus(http_request_status::FINISHED);
	else if (request.getContentLength() < request.getBodySize()) {
		request.setStatus(http_request_status::ERROR);
		request.setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	return (0);
}

int HttpRequestParser::parseChunkedBody(HttpRequest& request, std::string const& line) {
	if (request.getChunkedStatus() == chunked_status::CHUNKED_SIZE) {
		// TODO no error handling
		request.setChunkedSize(std::strtol(line.c_str(), NULL, 16));
		request.setChunkedStatus(chunked_status::CHUNKED_MESSAGE);
	} else if (line.size() == 0 && request.getChunkedSize() == 0) {
		request.setStatus(http_request_status::FINISHED);
	} else {
		if (request.getChunkedSize() != line.size()) {
			request.setStatus(http_request_status::ERROR);
			request.setErrorStatus(http_error_status::BAD_REQUEST);
			return (-1);
		}
		request.addBody(line);
		request.setChunkedStatus(chunked_status::CHUNKED_SIZE);
	}
	return (0);
}
int HttpRequestParser::checkHeaderValue(HttpRequest& request) {
	std::string method = request.getMethod();
	if (method == "GET" || method == "DELETE") {
		request.setBodyMassageType(http_body_message_type::NONE);
		request.setStatus(http_request_status::FINISHED);
	} else if (method == "POST") {
		if (request.getHeaderValue("Transfer-Encoding") == "chunked")
			request.setBodyMassageType(http_body_message_type::CHUNK_ENCODING);
		else if (request.getHeaderValue("Content-Length") != "") {
			request.setBodyMassageType(http_body_message_type::CONTENT_LENGTH);
			// TODO you should check the value is affordable.
			request.setContentLength(
				static_cast<size_t>(std::atoi(request.getHeaderValue("Content-Length").c_str())));
		}
		request.setStatus(http_request_status::BODY);
	} else {
		request.setStatus(http_request_status::ERROR);
		request.setErrorStatus(http_error_status::BAD_REQUEST);
		return (-1);
	}
	return (0);
}
}
