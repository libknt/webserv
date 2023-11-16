#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include "http_request.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <new>
#include <string>
#include <vector>

namespace server {

enum META_VARIABLES {
	PATH_INFO_META_VARIABLE,
	PATH_TRANSLATED_META_VARIABLE,
	QUERY_STRING_META_VARIABLE,
	SCRIPT_NAME_META_VARIABLE,
};

class CgiMetaVariables {
private:
	CgiMetaVariables();
	void setMetaVariables(std::string key, std::string value);

	const HttpRequest& request_;
	std::map<std::string, std::string> meta_variables_;
	char** cgi_environ_;

	typedef int (CgiMetaVariables::*MetaVariableFunc)();
	int authType();
	int contentLength();
	int contentType();
	int gatewayInterface();
	int pathInfo();
	int pathTranslated();
	int queryString();
	int remoteAddr();
	int remoteHost();
	int remoteIdet();
	inline bool isBase64(unsigned char c);
	std::string base64Decode(std::string const& encoded_string);
	int remoteUser();
	int requestMethod();
	int scriptName();
	int serverName();
	int serverPort();
	int serverProtocol();
	int serverSoftware();

	int requestPathParse(std::string request_path,
		std::string& parsed_line,
		META_VARIABLES what_meta_variable);
	std::string extractQuery(std::string& path);
	std::string extractScriptName(std::string& path);
	std::string extractPathInfo(std::string& path);
	std::string extractPathTranslated(std::string& path);

public:
	CgiMetaVariables(const HttpRequest& request);
	CgiMetaVariables(const CgiMetaVariables& other);
	CgiMetaVariables& operator=(const CgiMetaVariables& other);
	~CgiMetaVariables();

	int setup();
	int createCgiEnviron();
	const std::map<std::string, std::string>& getMetaVariables() const;
	char** getCgiEnviron() const;
	const std::string& getMetaVariables(std::string key) const;
};

std::ostream& operator<<(std::ostream& out, const CgiMetaVariables& cgi_meta_variables);

char** DeepCopyCharPointerArray(char** source);
} // namespace server

#endif