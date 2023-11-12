#ifndef CGI_HPP
#define CGI_HPP

#include "cgi_meta_variables.hpp"
#include "http_request.hpp"

namespace server {

class Cgi {
private:
	Cgi();

	const HttpRequest& request_;
	CgiMetaVariables meta_variables_;

public:
	Cgi(const HttpRequest& request);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setup();
};

} // namespace cgi

#endif