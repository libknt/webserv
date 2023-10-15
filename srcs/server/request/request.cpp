#include <iostream>
#include <string>
#include <map>

enum    http_status
{
	Method,
	Header,
	Body,
	Finished,
	Error,
};

enum    http_method
{
	GET,
	POST,
	DELETE,
};

enum http_version
{
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0,
}

class   HttpRequest
{
	public:
		HttpRequest(int status);
		int									parseHttpRequest(std::string line);
		std::string							getHeaderValue(std::string key);
	private:
		int									method;
		int									status;
		int									version;
		int									error;
		std::string							request_path;
		std::map<std::string, std::string>	query;
		std::map<std::string, std::string>	header;
		std::string							body;
};

HttpRequest::HttpRequest(int status) : status(status)
{

}

int		HttpRequest::parseHttpRequest(std::string line)
{
	if (this->status == Method)
		parseHttpMethod(line);
	else if (this->status == Header)
		parseHttpHeader(line);
	else if (this->status == Body)
		parseHttpBody(line);
	if (this->status == Error)
		return (1);
	return (0);
}

int	HttpRequest::parseHttpMethod(std::string line)
{
	if (strncmp(line.c_str(),"GET", 3) == 0)
		this->method = GET;
	else if (strncmp(line.c_str(),"POST", 4) == 0)
		this->method = POST;
	else if (strncmp(line.c_str(),"DELETE", 6) == 0)
		this->method = DELETE;
}