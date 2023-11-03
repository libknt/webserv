#ifndef CGI_MRTA_VARIABLES_HPP
#define CGI_MRTA_VARIABLES_HPP

#include "http_request.hpp"
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <new>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

class HttpRequest;
enum URL_META_VARIABLES {
	SCRIPT_NAME,
	PATH_INFO,
	PATH_TRANSLATED,
	QUERY_STRING,
};

namespace server {

// https://tex2e.github.io/rfc-translater/html/rfc3875.html#4-1--Request-Meta-Variables

class CgiMetaVariables {
private:
	HttpRequest& request_;
	std::map<std::string, std::string> meta_variables_;
	typedef int (CgiMetaVariables::*MetaFuncPtr)();
	std::vector<MetaFuncPtr> metaFuncArray;
	char** exec_environ_;
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
	int remoteUser();
	int requestMethod();
	int scriptName();
	int serverName();
	int serverPort();
	int serverProtocol();
	int serverSoftware();
	int setMetaVariables();

	static inline bool isBase64(unsigned char c);
	static std::string base64Decode(std::string const& encoded_string);
	int urlParse(std::string request_path, std::string& parsed_line, URL_META_VARIABLES what);
	std::string extractScriptName(std::string& path);
	std::string extractQuery(std::string& path);
	std::string extractPathInfo(std::string& path);
	std::string extractAfterCgiBin(std::string& request_path);

	explicit CgiMetaVariables();

public:
	explicit CgiMetaVariables(HttpRequest& request);
	explicit CgiMetaVariables(const CgiMetaVariables& other);
	CgiMetaVariables& operator=(const CgiMetaVariables& other);
	virtual ~CgiMetaVariables();
	int createMetaVariables();
	void getMeta();
	char** getExecEnviron() const;
	std::string findMetaVariable(std::string key);
};

}
#endif