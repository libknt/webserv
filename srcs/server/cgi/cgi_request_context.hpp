#ifndef CGI_REQUEST_CONTEXT_HPP
#define CGI_REQUEST_CONTEXT_HPP

#include "http_request.hpp"
#include <cstring>
#include <map>
#include <string>

namespace server {

class CgiRequestContext {
private:
	HttpRequest const& request_;
	std::map<std::string, std::string> meta_variables_;
	sockaddr_in const& client_address_;
	sockaddr_in const& server_address_;
	char** environ_;

	typedef int (CgiRequestContext::*MetaVariableFunc)();

	CgiRequestContext();
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
	CgiRequestContext(HttpRequest const& request,
		sockaddr_in const& client_address,
		sockaddr_in const& server_address);
	CgiRequestContext(CgiRequestContext const& other);
	CgiRequestContext& operator=(CgiRequestContext const& other);
	~CgiRequestContext();
	int setup();
	int setupCgiMetaVariables();
	int createEnviron();
	char** getCgiEnviron() const;
	std::string const getMetaVariable(std::string const& key) const;
};

std::ostream& operator<<(std::ostream& out, const CgiRequestContext& cgi_meta_variables);

char** DeepCopyCharPointerArray(char** source);

} // namespace server

#endif
