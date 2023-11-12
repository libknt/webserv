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
	typedef int (CgiMetaVariables::*MetaFuncPtr)();
	std::vector<MetaFuncPtr> metaFuncArray;

public:
	CgiMetaVariables(const HttpRequest& request);
	CgiMetaVariables(const CgiMetaVariables& other);
	CgiMetaVariables& operator=(const CgiMetaVariables& other);
	~CgiMetaVariables();

	int setup();
};

} // namespace server

#endif