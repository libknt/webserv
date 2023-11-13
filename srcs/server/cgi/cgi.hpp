#ifndef CGI_HPP
#define CGI_HPP

#include "cgi_meta_variables.hpp"
#include "http_request.hpp"
#include <cstring>
#include <errno.h>

namespace server {

class Cgi {
private:
	Cgi();
	int setupInterProcessCommunication();

	const HttpRequest& request_;
	CgiMetaVariables meta_variables_;
	int socket_vector_[2];
	bool has_body;

public:
	Cgi(const HttpRequest& request);
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
	int setup();
	bool hasBody() const;
	bool getHasbody() const;
};

} // namespace cgi

#endif