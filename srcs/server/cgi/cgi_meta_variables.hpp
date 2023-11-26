#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include "http_request.hpp"
#include <map>
#include <string>

namespace server {

class CgiMetaVariables {
private:
	HttpRequest const& request_;
	std::map<std::string, std::string> meta_variables_;
	char** environ_;

	CgiMetaVariables();
	void setMetaVariables(std::string const& key, std::string const& value);

public:
	CgiMetaVariables(HttpRequest const& request);
	CgiMetaVariables(CgiMetaVariables const& other);
	CgiMetaVariables& operator=(CgiMetaVariables const& other);
	~CgiMetaVariables();
};

} // namespace server

#endif
