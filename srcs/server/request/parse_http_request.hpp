#ifndef PARSE_HTTP_REQUEST_HPP
#define PARSE_HTTP_REQUEST_HPP

#include "http_request.hpp"
#include "struct.hpp"
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
	RequestProcessStatus handleBuffer(int socketfd, char* buf);
	HttpRequest& getHttpRequest(int sd);
	void addAcceptClientInfo(int socketfd, sockaddr_in client_address, sockaddr_in server_address);
	void httpRequestErase(int sd);
	void printAcceptedFds();
};

}
#endif
