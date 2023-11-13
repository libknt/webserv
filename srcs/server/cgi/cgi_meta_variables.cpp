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
	MetaVariableFunc functions[] = {
		&CgiMetaVariables::authType,
		&CgiMetaVariables::contentLength,
		&CgiMetaVariables::contentType,
		&CgiMetaVariables::gatewayInterface,
		&CgiMetaVariables::pathInfo,
		&CgiMetaVariables::pathTranslated,
		&CgiMetaVariables::queryString,
		&CgiMetaVariables::remoteAddr,
		&CgiMetaVariables::remoteHost,
		&CgiMetaVariables::remoteIdet,
		&CgiMetaVariables::remoteUser,
		&CgiMetaVariables::requestMethod,
		&CgiMetaVariables::scriptName,
		&CgiMetaVariables::serverName,
		&CgiMetaVariables::serverPort,
		&CgiMetaVariables::serverProtocol,
		&CgiMetaVariables::serverSoftware,
	};

	int funcSize = sizeof(functions) / sizeof(MetaVariableFunc);

	for (int i = 0; i < funcSize; ++i) {
		(this->*functions[i])();
	}

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

int CgiMetaVariables::pathInfo() {
	std::string path_info;
	if (requestPathParse(request_.getRequestPath(), path_info, PATH_INFO_META_VARIABLE) < 0) {
		return -1;
	}
	meta_variables_.insert(std::make_pair("PATH_INFO", path_info));
	return 0;
}

int CgiMetaVariables::pathTranslated() {
	std::string path_translated;
	if (requestPathParse(
			request_.getRequestPath(), path_translated, PATH_TRANSLATED_META_VARIABLE) < 0) {
		return -1;
	}
	// TODO serverのドキュメントルートに合わせる.
	// /var/www/html/additional/path/info
	meta_variables_.insert(std::make_pair("PATH_TRANSLATED", path_translated));
	return 0;
}

int CgiMetaVariables::queryString() {
	std::string query;
	if (requestPathParse(request_.getRequestPath(), query, QUERY_STRING_META_VARIABLE) < 0) {
		return -1;
	}
	meta_variables_.insert(std::make_pair("QUERY_STRING", query));
	return 0;
}

int CgiMetaVariables::remoteAddr() {
	sockaddr_in client_addr = request_.getClientAddress();
	uint32_t addr = ntohl(client_addr.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	setMetaVariables("REMOTE_ADDR", ip_stream.str());
	return 0;
}

int CgiMetaVariables::remoteHost() {
	// TODO 取得する関数が、使用可能関数にないと思う
	setMetaVariables("REMOTE_HOST", "");
	return 0;
}

int CgiMetaVariables::remoteIdet() {
	// TODO 推奨されてないので空
	setMetaVariables("REMOTE_IDENT", "");
	return 0;
}

inline bool CgiMetaVariables::isBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CgiMetaVariables::base64Decode(std::string const& encoded_string) {
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && isBase64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret.push_back(char_array_3[j]);
	}

	return ret;
}

int CgiMetaVariables::remoteUser() {
	std::string remote_user = request_.getHeaderValue("Authorization");
	if (remote_user.compare("") != 0) {
		remote_user = remote_user.substr(remote_user.find(' ') + 1);
		remote_user = base64Decode(remote_user);
		remote_user = remote_user.substr(0, remote_user.find(':'));
	}
	setMetaVariables("REMOTE_USER", remote_user);
	return 0;
}

int CgiMetaVariables::requestMethod() {
	std::string method = request_.getMethod();
	setMetaVariables("REQUEST_METHOD", method);
	return 0;
}

int CgiMetaVariables::scriptName() {
	std::string script_name;
	if (requestPathParse(request_.getRequestPath(), script_name, SCRIPT_NAME_META_VARIABLE) < 0) {
		return -1;
	}
	setMetaVariables("SCRIPT_NAME", script_name);
	return 0;
}

int CgiMetaVariables::serverName() {
	sockaddr_in server_addr = request_.getServerAddress();

	uint32_t addr = ntohl(server_addr.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	setMetaVariables("SERVER_NAME", ip_stream.str());
	return 0;
}

int CgiMetaVariables::serverPort() {
	sockaddr_in server_addr = request_.getServerAddress();
	int port = ntohs(server_addr.sin_port);
	std::stringstream ss;
	ss << port;
	std::string port_str = ss.str();
	setMetaVariables("SERVER_PORT", port_str);
	return 0;
}

int CgiMetaVariables::serverProtocol() {
	std::string protocol = request_.getVersion();
	setMetaVariables("SERVER_PROTOCOL", protocol);
	return 0;
}

int CgiMetaVariables::serverSoftware() {
	setMetaVariables("SERVER_SOFTWARE", "webserv/1.0");
	return 0;
}

int CgiMetaVariables::requestPathParse(std::string request_path,
	std::string& parsed_line,
	META_VARIABLES what_meta_variable) {
	switch (what_meta_variable) {
		break;
		case PATH_INFO_META_VARIABLE:
			parsed_line = extractPathInfo(request_path);
			break;
		case PATH_TRANSLATED_META_VARIABLE:
			parsed_line = extractPathTranslated(request_path);
			break;
		case QUERY_STRING_META_VARIABLE:
			parsed_line = extractQuery(request_path);
			break;
		case SCRIPT_NAME_META_VARIABLE:
			parsed_line = extractScriptName(request_path);
			break;
		default:
			return -1;
	}
	return 0;
}

std::string CgiMetaVariables::extractQuery(std::string& path) {
	std::string query;
	query = path;
	return query;
}

std::string CgiMetaVariables::extractScriptName(std::string& path) {
	std::string query;
	query = path;
	return query;
}

std::string CgiMetaVariables::extractPathInfo(std::string& path) {
	std::string query;
	query = path;
	return query;
}

std::string CgiMetaVariables::extractPathTranslated(std::string& path) {
	std::string query;
	query = path;
	return query;
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