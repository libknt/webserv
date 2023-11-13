#include "cgi_meta_variables.hpp"

namespace server {

CgiMetaVariables::CgiMetaVariables(const HttpRequest& request)
	: request_(request)
	, meta_variables_() {}

CgiMetaVariables::~CgiMetaVariables() {}

CgiMetaVariables::CgiMetaVariables(const CgiMetaVariables& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_) {}

CgiMetaVariables& CgiMetaVariables::operator=(const CgiMetaVariables& other) {
	if (this != &other) {
		meta_variables_ = other.meta_variables_;
	}
	return *this;
}

int CgiMetaVariables::setup() {
	authType();
	contentLength();
	contentType();
	gatewayInterface();
	remoteAddr();
	remoteHost();
	remoteIdet();
	return 0;
}

void CgiMetaVariables::setMetaVariables(std::string key, std::string value) {
	meta_variables_.insert(std::make_pair(key, value));
}

const std::map<std::string, std::string>& CgiMetaVariables::getMetaVariables() const {
	return meta_variables_;
}

int CgiMetaVariables::authType() {
	std::string auth_type = request_.getHeaderValue("Authorization");
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	setMetaVariables("AUTH_TYPE", auth_type);
	return 0;
}

int CgiMetaVariables::contentLength() {
	setMetaVariables("CONTENT_LENGTH", request_.getHeaderValue("Content-Length"));
	return 0;
}

int CgiMetaVariables::contentType() {
	setMetaVariables("CONTENT_TYPE", request_.getHeaderValue("Content-Type"));
	return 0;
}

int CgiMetaVariables::gatewayInterface() {
	setMetaVariables("GATEWAY_INTERFACE", "CGI/1.1");
	return 0;
}

int CgiMetaVariables::remoteAddr() {
	sockaddr_in client_addr = request_.getClientAddress();
	uint32_t addr = ntohl(client_addr.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	meta_variables_.insert(std::make_pair("REMOTE_ADDR", ip_stream.str()));
	return 0;
}

int CgiMetaVariables::remoteHost() {
	// TODO 取得する関数が、使用可能関数にないと思う
	meta_variables_.insert(std::make_pair("REMOTE_HOST", ""));
	return 0;
}

int CgiMetaVariables::remoteIdet() {
	// TODO 推奨されてないので空
	meta_variables_.insert(std::make_pair("REMOTE_IDENT", ""));
	return 0;
}

std::ostream& operator<<(std::ostream& out, const CgiMetaVariables& cgi_meta_variables) {
	const std::map<std::string, std::string>& meta_variables =
		cgi_meta_variables.getMetaVariables();

	out << "CgiMetaVariables: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = meta_variables.begin();
		 it != meta_variables.end();
		 ++it) {
		out << it->first << ": " << it->second << std::endl;
	}
	return out;
}

} // namespace server