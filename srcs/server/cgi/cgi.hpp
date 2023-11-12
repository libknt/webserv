#ifndef CGI_HPP
#define CGI_HPP

#include "http_request.hpp"

namespace server {

class Cgi {
private:
	Cgi();

	const HttpRequest& request_;

public:
	Cgi(const HttpRequest& request);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
};

} // namespace cgi

#endif