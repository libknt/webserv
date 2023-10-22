#include "http_request.hpp"
#include "parse_sentense.hpp"
#include <cstdlib>

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
	{
		setStatus(ERROR);
		return (-1);
	}
	return (0);
}

int		HttpRequest::setRequestPath(std::string const &request_path)
{
	if (request_path == "\0")
	{
		setStatus(ERROR);
		return (-1);
	}
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
	{
		setStatus(ERROR);
		return (-1);
	}
	return (0);
}

//Parse
int		HttpRequest::parseHttpRequest(std::string const &line)
{
	if (this->status_ == METHOD)
		parseHttpMethod(line);
	else if (this->status_ == HEADER)
		parseHttpHeader(line);
	else if (this->status_ == BODY)
		parseHttpBody(line);
	if (this->status_ == ERROR)
		return (-1);
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

int		HttpRequest::parseHttpHeader(std::string const &line)
{
	std::vector<std::string>	header_vector;
	if (line == "\0")
	{	
		if (method_ == GET || getHeaderValue("Content-Length") == "0")
			setStatus(FINISHED);
		else
			setStatus(BODY);
		getInfo();
		return (0);
	}
	//Later: http request compromise 0 space between : and value!
	else if (parse_sentense(line, "%s: %s", header_vector) < 0 || header_vector.size() != 2)
	{
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	if (setHeaderValue(header_vector[0], header_vector[1]) < 0)
	{
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (-1);
	}
	return (0);
}

int		HttpRequest::setHeaderValue(std::string const &key, std::string const &value)
{
	this->header_[key] = value;
	return (0);
}

std::string	HttpRequest::getHeaderValue(std::string const &key)
{
	if (this->header_.count(key) == 0)
		return (std::string(""));
	else
		return (this->header_[key]);
}

int		HttpRequest::parseHttpBody(std::string const &line)
{
	//transfer-encoding
	if (getHeaderValue("Transfer-Encoding") == "chunked")
		return (parseChunkedBody(line));
	//content-length
	else if (getHeaderValue("Content-Length") != "")
		return (parseContentLengthBody(line));
	setStatus(ERROR);
	setErrorStatus(BAD_REQUEST);
	return (-1);
}

int		HttpRequest::parseChunkedBody(std::string const &line)
{
	if (chunked_status_ == CHUNKED_SIZE)
	{
		// no error handling
		//Caution ! strtol is C++ 11 function.
		chunked_size_ = std::strtol(line.c_str(), NULL, 16);
		chunked_status_ = CHUNKED_MESSAGE;
		if (chunked_size_ == 0)
			setStatus(FINISHED);
	}
	else if (line == "")
		chunked_status_ = CHUNKED_SIZE;
	else
		body_ += line; 
	return (0);
}
		
int		HttpRequest::parseContentLengthBody(std::string const &line)
{
	//no error handling
	body_ += line;
	if (body_.size() == static_cast<size_t>(std::atoi(getHeaderValue("Content-Length").c_str())))
		setStatus(FINISHED);
	return (0);
}

void	HttpRequest::getInfo(void)
{
	std::cout << "method: " << method_ << std::endl;
	std::cout << "status: " << status_ << std::endl;
	std::cout << "version: " << version_ << std::endl;
	std::cout << "header" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = header_.begin(); iter != header_.end(); ++iter)
		std::cout << "first: " << iter->first << "second: " << iter->second << std::endl;
}

