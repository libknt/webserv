#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

namespace server {

class cgiMetaVariables {
private:
	cgiMetaVariables();
	cgiMetaVariables(const cgiMetaVariables& other);
	cgiMetaVariables& operator=(const cgiMetaVariables& other);
	~cgiMetaVariables();
}

} // namespace server

#endif