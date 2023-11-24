#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include <map>
#include <string>

class CgiMetaVariables {
private:
	std::map<std::string, std::string> meta_variables_;
	void setMetaVariables(std::string const& key, std::string const& value);

public:
	CgiMetaVariables();
	CgiMetaVariables(CgiMetaVariables const& other);
	CgiMetaVariables& operator=(CgiMetaVariables const& other);
	~CgiMetaVariables();
};

#endif
