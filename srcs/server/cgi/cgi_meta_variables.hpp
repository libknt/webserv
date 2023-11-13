#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include "http_request.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace server {

class CgiMetaVariables {
private:
	CgiMetaVariables();
	void setMetaVariables(std::string key, std::string value);

	const HttpRequest& request_;
	std::map<std::string, std::string> meta_variables_;

	int authType();
	int contentLength();

public:
	CgiMetaVariables(const HttpRequest& request);
	CgiMetaVariables(const CgiMetaVariables& other);
	CgiMetaVariables& operator=(const CgiMetaVariables& other);
	~CgiMetaVariables();

	int setup();
	const std::map<std::string, std::string>& getMetaVariables() const;
};

std::ostream& operator<<(std::ostream& out, const CgiMetaVariables& cgi_meta_variables);

} // namespace server

#endif