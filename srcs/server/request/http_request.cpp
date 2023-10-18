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
	if (parse_sentense(line, "%s %s %s", method_vector) == -1)
	{
		std::cout << "Http Method Parse Error" << std::endl;
		setStatus(ERROR);
		setErrorStatus(BAD_REQUEST);
		return (1);
	}
	setStatus(HEADER);
	return (0);
}
