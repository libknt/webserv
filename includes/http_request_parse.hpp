#ifndef	HTTP_REQUEST_PARSE_HPP
#define	HTTP_REQUEST_PARSE_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "http_request.hpp"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class	HttpRequestParse
{
	public:
		int							parse_http_request(int socketfd, char *buf);
	private:
		std::map<int, HttpRequest>	http_request_map_;
		std::map<int, std::string>	http_line_stream_;
};

#endif
