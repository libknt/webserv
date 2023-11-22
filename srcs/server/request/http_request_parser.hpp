#ifndef PARSE_HTTP_REQUEST_HPP
#define PARSE_HTTP_REQUEST_HPP

#include "http_request.hpp"
#include "types.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

namespace server {

namespace parse_request_line {
enum PARSE_REQUEST_LINE {
	METHOD,
	REQUEST_PATH,
	VERSION,
};
}

namespace parse_header {
enum PARSE_HEADER {
	KEY,
	VALUE,
};
}

class HttpRequestParser {
private:
	std::map<int, HttpRequest> http_request_map_;
	std::map<int, std::string> http_line_stream_;
	int parseRequest(int sd, std::string const& line);
	int parseStartLine(HttpRequest& request, std::string const& line);
	int parseHeader(HttpRequest& request, std::string const& line);
	int parseBody(HttpRequest& request, std::string const& line);
	int parseContentLengthBody(HttpRequest& request, std::string const& line);
	int parseChunkedBody(HttpRequest& request, std::string const& line);
	int checkHeaderValue(HttpRequest& request);

public:
	explicit HttpRequestParser();
	explicit HttpRequestParser(HttpRequestParser& other);
	virtual ~HttpRequestParser();
	HttpRequestParser& operator=(HttpRequestParser& other);
	SERVER_STATUS handleBuffer(int sd, const char* buf);
	HttpRequest& getRequest(int sd);
	int addAcceptClientInfo(int sd, sockaddr_in client_address, sockaddr_in server_address);
	int httpRequestCleanup(int sd);
	int httpRequestErase(int sd);
};

}
#endif
