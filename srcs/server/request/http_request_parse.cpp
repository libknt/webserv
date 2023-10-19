#include "http_request_parse.hpp"

int		HttpRequestParse::parse_http_request(int socketfd, char *buf)
{

	std::string				buffer(buf);
	HttpRequest				request;
	std::string::size_type	index;
	std::string				line;

	if (http_request_map_.find(socketfd) == http_request_map_.end())
		http_request_map_[socketfd] = request;
	http_line_stream_[socketfd] += buffer; 
	while ((index = http_line_stream_[socketfd].find("\r\n")) < http_line_stream_[socketfd].size())
	{
			std::string		line = http_line_stream_[socketfd].substr(0, index);
			std::cout << line << std::endl;
			http_line_stream_[socketfd] = http_line_stream_[socketfd].substr(index + 2);
			http_request_map_[socketfd].parseHttpRequest(line);
			http_request_map_[socketfd].getInfo();
	}
	return (0);
}




