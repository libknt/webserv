#include <iostream>
#include <map>
#include <vector>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class	HttpRequestParse
{
	public:
		parse_http_request(int socketfd, char *buffer);
	private:
		std::map<int, HttpRequest>	http_request_map;
};
