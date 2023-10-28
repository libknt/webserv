#include "cgi_meta_variables.hpp"

namespace server {
CgiMetaVariables::CgiMetaVariables(HttpRequest& request)
	: request_(request)
	, exec_environ_(NULL) {
	metaFuncArray.push_back(&CgiMetaVariables::authType);
	metaFuncArray.push_back(&CgiMetaVariables::contentLength);
	metaFuncArray.push_back(&CgiMetaVariables::contentType);
	metaFuncArray.push_back(&CgiMetaVariables::gatewayInterface);
	metaFuncArray.push_back(&CgiMetaVariables::pathInfo);
	metaFuncArray.push_back(&CgiMetaVariables::pathTranslated);
	metaFuncArray.push_back(&CgiMetaVariables::queryString);
	metaFuncArray.push_back(&CgiMetaVariables::remoteAddr);
	metaFuncArray.push_back(&CgiMetaVariables::remoteHost);
	metaFuncArray.push_back(&CgiMetaVariables::remoteUser);
	metaFuncArray.push_back(&CgiMetaVariables::requestMethod);
	metaFuncArray.push_back(&CgiMetaVariables::scriptName);
	metaFuncArray.push_back(&CgiMetaVariables::serverName);
	metaFuncArray.push_back(&CgiMetaVariables::serverPort);
	metaFuncArray.push_back(&CgiMetaVariables::serverProtocol);
	metaFuncArray.push_back(&CgiMetaVariables::serverSoftware);
}

CgiMetaVariables::CgiMetaVariables(const CgiMetaVariables& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_) {}

CgiMetaVariables& CgiMetaVariables::operator=(const CgiMetaVariables& other) {
	if (this != &other) {
		this->meta_variables_ = other.meta_variables_;
	}
	return *this;
}

CgiMetaVariables::~CgiMetaVariables() {
	delete[] exec_environ_;
}

int CgiMetaVariables::authType() {
	std::string auth_type = request_.getHeaderValue("Authorization");
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	meta_variables_.insert(std::make_pair("AUTH_TYPE", auth_type));
	return 0;
}

int CgiMetaVariables::contentLength() {
	meta_variables_.insert(
		std::make_pair("CONTENT_LENGTH", request_.getHeaderValue("Content-Length")));
	return 0;
}

int CgiMetaVariables::contentType() {
	meta_variables_.insert(std::make_pair("CONTENT_TYPE", request_.getHeaderValue("Content-Type")));
	return 0;
}

int CgiMetaVariables::gatewayInterface() {
	meta_variables_.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	return 0;
}

int CgiMetaVariables::pathInfo() {
	std::string path_info;
	urlParse(request_.getRequestPath(), path_info, PATH_INFO);
	meta_variables_.insert(std::make_pair("PATH_INFO", path_info));
	return 0;
}

int CgiMetaVariables::pathTranslated() {
	std::string path_translated;
	urlParse(request_.getRequestPath(), path_translated, PATH_TRANSLATED);
	// TODO serverのドキュメントルートに合わせる.
	// /var/www/html/additional/path/info
	meta_variables_.insert(std::make_pair("PATH_TRANSLATED", path_translated));
	return 0;
}

int CgiMetaVariables::queryString() {
	std::string query;
	urlParse(request_.getRequestPath(), query, QUERY_STRING);
	meta_variables_.insert(std::make_pair("QUERY_STRING", query));
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
	meta_variables_.insert(std::make_pair("REMOTE_USER", remote_user));
	return 0;
}

int CgiMetaVariables::requestMethod() {
	std::string method = request_.getHttpMethod();
	meta_variables_.insert(std::make_pair("REQUEST_METHOD", method));
	return 0;
}

int CgiMetaVariables::scriptName() {
	std::string script_name;
	urlParse(request_.getRequestPath(), script_name, SCRIPT_NAME);
	meta_variables_.insert(std::make_pair("SCRIPT_NAME", script_name));
	return 0;
}

int CgiMetaVariables::serverName() {
	sockaddr_in server_addr = request_.getServerAddress();

	uint32_t addr = ntohl(server_addr.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	meta_variables_.insert(std::make_pair("SERVER_NAME", ip_stream.str()));
	return 0;
}

int CgiMetaVariables::serverPort() {
	sockaddr_in server_addr = request_.getServerAddress();
	int port = ntohs(server_addr.sin_port);
	std::stringstream ss;
	ss << port;
	std::string port_str = ss.str();
	meta_variables_.insert(std::make_pair("SERVER_PORT", port_str));
	return 0;
}

int CgiMetaVariables::serverProtocol() {
	std::string protocol = request_.getServerProtocol();
	meta_variables_.insert(std::make_pair("SERVER_PROTOCOL", protocol));
	return 0;
}

int CgiMetaVariables::serverSoftware() {
	meta_variables_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	return 0;
}

int CgiMetaVariables::createMetaVariables() {
	for (std::vector<MetaFuncPtr>::iterator it = metaFuncArray.begin(); it != metaFuncArray.end();
		 ++it) {
		if ((this->**it)() < 0)
			return -1;
	}
	setMetaVariables();
	return 0;
}

int CgiMetaVariables::setMetaVariables() {
	// TODO malloc error
	int index = 0;
	exec_environ_ = new (std::nothrow) char*[meta_variables_.size() + 1];
	if (!exec_environ_) {
		return -1;
	}

	for (std::map<std::string, std::string>::iterator it = meta_variables_.begin();
		 it != meta_variables_.end();
		 ++it) {
		std::string env = it->first + "=" + it->second;
		exec_environ_[index] = strdup(env.c_str());
		// TODO malloc err
		if (exec_environ_[index] == NULL) {
			return -1;
		}
		++index;
	}
	exec_environ_[index] = NULL;
	return 0;
}

void CgiMetaVariables::getMeta() {
	for (int i = 0;; ++i) {
		if (exec_environ_[i] == NULL)
			break;
		std::cout << exec_environ_[i] << std::endl;
	}
}

std::string CgiMetaVariables::extractAfterCgiBin(std::string& request_path) {
	size_t pos = request_path.find("/cgi-bin/");
	if (pos == std::string::npos) {
		std::cerr << "not found request_path[cgi-bin]" << std::endl;
		return "";
	}
	return request_path.substr(pos);
}

std::string CgiMetaVariables::extractPathInfo(std::string& path) {

	size_t pos = path.find('.');
	if (pos == std::string::npos) {
		std::cerr << "not found [cgi-script]" << std::endl;
		return "";
	} else {
		size_t query_pos = path.find('?', pos);
		if (query_pos != std::string::npos) {
			path = path.substr(0, query_pos);
		}
		size_t path_info_pos = path.find('/', pos);
		if (path_info_pos != std::string::npos) {
			path = path.substr(path_info_pos);
		} else {
			path = "";
		}
		return path;
	}
}

std::string CgiMetaVariables::extractQuery(std::string& path) {

	size_t pos = path.find('.');
	if (pos == std::string::npos) {
		std::cerr << "not found [cgi-script]" << std::endl;
		return "";
	} else {
		size_t query_pos = path.find('?', pos);
		if (query_pos != std::string::npos) {
			path = path.substr(query_pos + 1);
		}
		return path;
	}
}

std::string CgiMetaVariables::extractScriptName(std::string& path) {
	size_t pos = path.find('.');
	if (pos == std::string::npos) {
		std::cerr << "not found [cgi-script]" << std::endl;
		return "";
	} else {
		size_t query_pos = path.find('?', pos);
		if (query_pos != std::string::npos) {
			path = path.substr(0, query_pos);
		}
		size_t path_info_pos = path.find('/', pos);
		if (path_info_pos != std::string::npos) {
			path = path.substr(0, path_info_pos);
		}
		return path;
	}
}

int CgiMetaVariables::urlParse(std::string request_path,
	std::string& parsed_line,
	URL_META_VARIABLES what) {
	std::string path_after_cgi = extractAfterCgiBin(request_path);
	if (path_after_cgi.empty())
		return -1;

	// TODO queryが%hhの場合decodeするのかどうか調べて実装。
	switch (what) {
		case SCRIPT_NAME:
			parsed_line = extractScriptName(path_after_cgi);
			break;
		case PATH_INFO:
			parsed_line = extractPathInfo(path_after_cgi);
			break;
		case PATH_TRANSLATED:
			parsed_line = extractPathInfo(path_after_cgi);
			break;
		case QUERY_STRING:
			parsed_line = extractQuery(path_after_cgi);
			break;
		default:
			parsed_line = std::string("");
	}

	return 0;
}

char** CgiMetaVariables::getExecEnviron() const {
	return exec_environ_;
}

std::string CgiMetaVariables::findMetaVariable(std::string key) {

	std::string meta_variable;
	for (int i = 0; exec_environ_[i] != NULL; ++i) {
		if (std::strncmp(exec_environ_[i], key.c_str(), std::strlen(key.c_str())) == 0) {
			meta_variable = exec_environ_[i];
		}
	}
	std::string::size_type pos = meta_variable.find('=');
	if (pos != std::string::npos) {
		meta_variable = meta_variable.substr(pos + 1);
	} else {
		meta_variable = "";
	}
	return meta_variable;
}
}
