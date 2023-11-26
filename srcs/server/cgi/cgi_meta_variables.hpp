#ifndef CGI_META_VARIABLES_HPP
#define CGI_META_VARIABLES_HPP

#include "http_request.hpp"
#include <cstring>
#include <map>
#include <string>

namespace server {

class CgiMetaVariables {
private:
	HttpRequest const& request_;
	std::map<std::string, std::string> meta_variables_;
	sockaddr_in const& client_address_;
	sockaddr_in const& server_address_;
	char** environ_;

	CgiMetaVariables();
	void setMetaVariables(std::string const& key, std::string const& value);
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

public:
	CgiMetaVariables(HttpRequest const& request,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address);
	CgiMetaVariables(CgiMetaVariables const& other);
	CgiMetaVariables& operator=(CgiMetaVariables const& other);
	~CgiMetaVariables();
	std::string const getMetaVariable(std::string const& key) const;
};

} // namespace server

#endif
