#include "http_request_parser.hpp"
namespace server {

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequestParser::HttpRequestParser(HttpRequestParser& other) {
	*this = other;
}

HttpRequestParser& HttpRequestParser::operator=(HttpRequestParser& other) {
	(void)other;
	return (*this);
}

void HttpRequestParser::parse(HttpRequest& request, const char* buf) {

	std::string buffer(buf);
	std::string::size_type index;

	request.appendStreamLine(buffer);
	while ((index = request.getStreamLine().find("\r\n")) != std::string::npos) {
		if (request.getStatus() == http_request_status::BODY && request.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH)
			break;
		std::string line = request.getStreamLine().substr(0, index);
		request.eraseStreamLine(0, index + 2);
		if (parseRequest(request, line) == -1) {
			return;
		}
	}

	if (request.getStatus() == http_request_status::BODY &&
		request.getBodyMessageType() == http_body_message_type::CONTENT_LENGTH) {
		if (parseRequest(request, request.getStreamLine()) == -1) {
			return;
		}
		request.setStreamLine("");
	}
}

int HttpRequestParser::parseRequest(HttpRequest& request, std::string const& line) {
	switch (request.getStatus()) {
		case http_request_status::METHOD:
			parseStartLine(request, line);
			break;
		case http_request_status::HEADER:
			parseHeader(request, line);
			break;
		case http_request_status::BODY:
			parseBody(request, line);
			break;
		default:
			request.setStatus(http_request_status::ERROR);
			break;
	}
	if (request.getStatus() == http_request_status::ERROR) {
		return (-1);
	}
	return (0);
}

int HttpRequestParser::parseStartLine(HttpRequest& request, std::string const& line) {

	size_t index = 0;
	parse_request_line::PARSE_REQUEST_LINE status = parse_request_line::METHOD;

	for (size_t i = 0; i < line.size(); i++) {
		if (line[i] == ' ') {
			if (status == parse_request_line::METHOD) {
				if (request.setMethod(line.substr(index, i - index)) < 0) {
					return (-1);
				}
				index = i + 1;
				status = parse_request_line::REQUEST_PATH;
			} else if (parse_request_line::REQUEST_PATH) {
				if (request.setRequestPath(line.substr(index, i - index)) < 0) {
					return (-1);
				}
				index = i + 1;
				status = parse_request_line::VERSION;
			} else {
				request.setStatus(http_request_status::ERROR);
				return (-1);
			}
		}
	}
	if (status != parse_request_line::VERSION ||
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
	request.appendBody(line);
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
		request.appendBody(line);
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
		if (request.getHeaderValue("transfer-encoding") == "chunked") {
			request.setBodyMassageType(http_body_message_type::CHUNK_ENCODING);
		} else if (request.getHeaderValue("content-length") != "") {
			request.setBodyMassageType(http_body_message_type::CONTENT_LENGTH);
			// TODO you should check the value is affordable.
			request.setContentLength(
				static_cast<size_t>(std::atoi(request.getHeaderValue("content-length").c_str())));
		} else {
			request.setStatus(http_request_status::ERROR);
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
