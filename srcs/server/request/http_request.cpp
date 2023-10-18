#include "http_request.hpp"
#include "parse_sentense.hpp"

HttpRequest::HttpRequest() : status_(METHOD)
{
}

//Getter and Setter

void	HttpRequest::setStatus(int const &status)
{
	this->status_ = status;
}

void	HttpRequest::setErrorStatus(int const &error_status)
{
	this->error_status_ = error_status;
}

int		HttpRequest::setMethod(std::string const &method)
{
	if (method == "GET")
		this->method_ = GET;
	else if (method == "POST")
		this->method_ = POST;
	else if (method == "DELETE")
		this->method_ = DELETE;
	else
		return (-1);
	return (0);
}

int		HttpRequest::setRequestPath(std::string const &request_path)
{
	if (request_path == "\0")
		return (-1);
	this->request_path_ = request_path;
	return (0);
}

int		HttpRequest::setVersion(std::string const &version)
{
	if (version == "HTTP/1.0")
		this->version_ = HTTP_1_0;
	else if (version == "HTTP/1.1")
		this->version_ = HTTP_1_1;
	else
		return (-1);
	return (0);
}

//Parse
int		HttpRequest::parseHttpRequest(std::string const &line)
{
	if (this->status_ == METHOD)
		parseHttpMethod(line);
//	else if (this->status == HEADER)
//		parseHttpHeader(line);
//	else if (this->status == BODY)
//		parseHttpBody(line);
//	if (this->status == ERROR)
//		return (1);
	return (0);
}

int	HttpRequest::parseHttpMethod(std::string const &line)
{
	std::vector<std::string>	method_vector;
	if (parse_sentense(line, "%s %s %s", method_vector) == -1 || method_vector.size() != 3)
	{
		std::cout << "Http Method Parse Error" << std::endl;
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	if (setMethod(method_vector[0]) < 0 || setRequestPath(method_vector[1]) < 0 || setVersion(method_vector[2]) < 0)
	{
		std::cout << "Http Method Parse Error" << std::endl;
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	setStatus(HEADER);
	return (0);
}

