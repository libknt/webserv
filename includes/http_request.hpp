#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include <iostream>
#include <map>
#include <string>
enum HTTP_REQUEST_STATUS {
	METHOD,
	HEADER,
	BODY,
	FINISHED,
	ERROR,
};

enum HTTP_METHOD {
	GET,
	POST,
	DELETE,
};

enum CHUNKED_STATUS {
	CHUNKED_SIZE,
	CHUNKED_MESSAGE,
};

enum HTTP_VERSION {
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0,
};

enum HTTP_ERROR {
	BAD_REQUEST = 400,
};

class HttpRequest {
private:
	HTTP_METHOD method_;
	HTTP_REQUEST_STATUS status_;
	HTTP_VERSION version_;
	HTTP_ERROR error_status_;
	CHUNKED_STATUS chunked_status_;
	long chunked_size_;
	std::string request_path_;
	std::map<std::string, std::string> header_;
	std::string body_;
	int parseHttpMethod(std::string const& line);
	int parseHttpHeader(std::string const& line);
	int parseHttpBody(std::string const& line);
	int parseContentLengthBody(std::string const& line);
	int parseChunkedBody(std::string const& line);
	int setMethod(std::string const& method);
	int setRequestPath(std::string const& request_path);
	int setVersion(std::string const& version);
	int setHeaderValue(std::string const& key, std::string const& value);
	void setStatus(HTTP_REQUEST_STATUS const& status);
	void setErrorStatus(HTTP_ERROR const& error_status);

public:
	HttpRequest();
	int parseHttpRequest(std::string const& line);
	std::string getHeaderValue(std::string const& key);
	void getInfo(void);
};
#endif
