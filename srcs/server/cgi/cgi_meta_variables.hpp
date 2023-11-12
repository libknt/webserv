#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include <map>
#include <string>

namespace server {

class cgiMetaVariables {
private:
	cgiMetaVariables();
	cgiMetaVariables(const cgiMetaVariables& other);
	cgiMetaVariables& operator=(const cgiMetaVariables& other);
	~cgiMetaVariables();

	std::map<std::string, std::string> meta_variables_;

public:
};

} // namespace server

#endif