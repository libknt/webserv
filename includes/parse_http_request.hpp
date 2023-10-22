#ifndef PARSE_HTTP_REQUEST_HPP
#define PARSE_HTTP_REQUEST_HPP

#include "http_request.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class HttpRequestParse {
public:
	int parse_http_request(int socketfd, char* buf);

private:
	std::map<int, HttpRequest> http_request_map_;
	std::map<int, std::string> http_line_stream_;
};

#endif
