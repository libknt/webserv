#ifndef PARSE_HTTP_REQUEST_HPP
#define PARSE_HTTP_REQUEST_HPP

#include "http_request.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>
#include "rules.hpp"

namespace server {

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class HttpRequestParser {
private:
	std::map<int, HttpRequest> http_request_map_;
	std::map<int, std::string> http_line_stream_;
	int parseRequest(int sd, std::string const &line);
	int parseMethod(int sd, std::string const &line);
	int parseHeader(int sd, std::string const &line);
	int checkHeaderValue(int sd);
	int parseBody(int sd, std::string const& line);
	int parseContentLengthBody(int sd, std::string const& line);
	int parseChunkedBody(int sd, std::string const& line);

public:
	explicit HttpRequestParser();
	explicit HttpRequestParser(HttpRequestParser& other);
	virtual ~HttpRequestParser();
	HttpRequestParser& operator=(HttpRequestParser& other);
	int handleBuffer(int sd, char const* buf);
	HttpRequest const& getRequest(int sd) const;
	void addAcceptClientInfo(int sd, sockaddr_in client_address, sockaddr_in server_address);
};

}
#endif
