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

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class ParseHttpRequest {
private:
	std::map<int, HttpRequest> http_request_map_;
	std::map<int, std::string> http_line_stream_;

public:
	explicit ParseHttpRequest();
	explicit ParseHttpRequest(ParseHttpRequest& other);
	virtual ~ParseHttpRequest();
	ParseHttpRequest& operator=(ParseHttpRequest& other);
	SERVER_STATUS handleBuffer(int socketfd, char* buf);
	HttpRequest& getHttpRequest(int sd);
	int addAcceptClientInfo(int socketfd, sockaddr_in client_address, sockaddr_in server_address);
	int httpRequestCleanup(int sd);
	int httpRequestErase(int sd);
	void printAcceptedFds();
};

}
#endif
