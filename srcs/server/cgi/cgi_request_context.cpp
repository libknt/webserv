#include "cgi_request_context.hpp"

namespace server {

CgiRequestContext::CgiRequestContext(HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: request_(request)
	, meta_variables_(std::map<std::string, std::string>())
	, client_address_(client_address)
	, server_address_(server_address)
	, execve_argv_(NULL)
	, environ_(NULL) {}

CgiRequestContext::CgiRequestContext(CgiRequestContext const& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_)
	, client_address_(other.client_address_)
	, server_address_(other.server_address_)
	, execve_argv_(other.execve_argv_)
	, environ_(other.environ_) {}

CgiRequestContext& CgiRequestContext::operator=(CgiRequestContext const& other) {
	if (this != &other) {
		meta_variables_ = meta_variables_;
		execve_argv_ = DeepCopyCharPointerArray(other.execve_argv_);
		environ_ = DeepCopyCharPointerArray(other.environ_);
	}
	return *this;
}

char** DeepCopyCharPointerArray(char** source) {
	if (!source) {
		return NULL;
	}

	size_t count = 0;
	while (source[count]) {
		++count;
	}

	char** destination = new (std::nothrow) char*[count + 1];
	if (!destination) {
		return NULL;
	}

	for (size_t i = 0; i < count; ++i) {
		size_t length = std::strlen(source[i]) + 1;
		destination[i] = new (std::nothrow) char[length];
		if (!destination[i]) {
			for (size_t j = 0; j < i; ++j) {
				delete[] destination[j];
			}
			delete[] destination;
			return NULL;
		}
		std::strcpy(destination[i], source[i]);
	}

	destination[count] = NULL;
	return destination;
}

CgiRequestContext::~CgiRequestContext() {

	if (execve_argv_) {
		for (size_t i = 0; execve_argv_[i]; ++i) {
			delete[] execve_argv_[i];
		}
		delete[] execve_argv_;
	}

	if (environ_) {
		for (size_t i = 0; environ_[i]; ++i) {
			delete[] environ_[i];
		}
		delete[] environ_;
	}
}

void CgiRequestContext::setMetaVariables(std::string const& key, std::string const& value) {
	meta_variables_.insert(std::make_pair(key, value));
}

int CgiRequestContext::authType() {
	std::string auth_type = request_.getHeaderValue("authorization");
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	setMetaVariables("AUTH_TYPE", auth_type);
	return 0;
}

int CgiRequestContext::contentLength() {
	setMetaVariables("CONTENT_LENGTH", request_.getHeaderValue("content-length"));
	return 0;
}

int CgiRequestContext::contentType() {
	setMetaVariables("CONTENT_TYPE", request_.getHeaderValue("content-type"));
	return 0;
}

int CgiRequestContext::gatewayInterface() {
	setMetaVariables("GATEWAY_INTERFACE", "CGI/1.1");
	return 0;
}

int CgiRequestContext::pathInfo() {
	const std::string path = request_.getUriPath();
	const std::string::size_type extension_position = path.find(".");

	if (extension_position != std::string::npos) {
		const std::string::size_type slash_position = path.find("/", extension_position + 1);
		if (slash_position != std::string::npos) {
			const std::string path_info = path.substr(slash_position);
			meta_variables_.insert(std::make_pair("PATH_INFO", path_info));
			return 0;
		}
	}

	meta_variables_.insert(std::make_pair("PATH_INFO", ""));
	return 0;
}

int CgiRequestContext::pathTranslated() {
	// TODO serverのドキュメントルートに合わせる.
	// サーバー上の実際のファイルパス
	// PATH_INFO
	// のパスがサーバーのドキュメントルートを基準とした物理的なファイルパスに変換されたもの.
	std::string path_info = getMetaVariable("PATH_INFO");
	std::string path_translated("");
	if (!path_info.empty()) {
		// root_path + path_info
		//  一旦
		path_translated = "/var/www/html/additional" + path_info;
	}
	meta_variables_.insert(std::make_pair("PATH_TRANSLATED", path_translated));
	return 0;
}

int CgiRequestContext::queryString() {
	std::string const query = request_.getUriQuery();
	meta_variables_.insert(std::make_pair("QUERY_STRING", query));
	return 0;
}

int CgiRequestContext::remoteAddr() {
	uint32_t addr = ntohl(client_address_.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	setMetaVariables("REMOTE_ADDR", ip_stream.str());
	return 0;
}

int CgiRequestContext::remoteHost() {
	// TODO 取得する関数が、使用可能関数にない
	setMetaVariables("REMOTE_HOST", "");
	return 0;
}

int CgiRequestContext::remoteIdet() {
	// TODO 推奨されてないので空
	setMetaVariables("REMOTE_IDENT", "");
	return 0;
}

inline bool CgiRequestContext::isBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CgiRequestContext::base64Decode(std::string const& encoded_string) {
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

int CgiRequestContext::remoteUser() {
	std::string remote_user = request_.getHeaderValue("authorization");
	if (remote_user.compare("") != 0) {
		remote_user = remote_user.substr(remote_user.find(' ') + 1);
		remote_user = base64Decode(remote_user);
		remote_user = remote_user.substr(0, remote_user.find(':'));
	}
	setMetaVariables("REMOTE_USER", remote_user);
	return 0;
}

int CgiRequestContext::requestMethod() {
	std::string method = request_.getMethod();
	setMetaVariables("REQUEST_METHOD", method);
	return 0;
}

int CgiRequestContext::scriptName() {
	std::string uri = request_.getUriPath();
	std::string script_name(uri);
	std::string path_info = getMetaVariable("PATH_INFO");
	if (!path_info.empty()) {
		uri.erase(path_info.size());
	}
	std::string::size_type position = uri.rfind("/");
	if (position != std::string::npos) {
		script_name.erase(position);
	}
	setMetaVariables("SCRIPT_NAME", script_name);
	return 0;
}

int CgiRequestContext::serverName() {

	uint32_t addr = ntohl(server_address_.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	setMetaVariables("SERVER_NAME", ip_stream.str());
	return 0;
}

int CgiRequestContext::serverPort() {
	int port = ntohs(server_address_.sin_port);
	std::stringstream ss;
	ss << port;
	std::string port_str = ss.str();
	setMetaVariables("SERVER_PORT", port_str);
	return 0;
}

int CgiRequestContext::serverProtocol() {
	std::string protocol = request_.getVersion();
	setMetaVariables("SERVER_PROTOCOL", protocol);
	return 0;
}

int CgiRequestContext::serverSoftware() {
	setMetaVariables("SERVER_SOFTWARE", "webserv/1.0");
	return 0;
}

int CgiRequestContext::setup() {
	if (setupCgiMetaVariables() < 0) {
		return -1;
	}
	if (setupExecveArgv() < 0) {
		return -1;
	}
	if (createEnviron() < 0) {
		return -1;
	}
	return 0;
}

int CgiRequestContext::setupExecveArgv() {
	execve_argv_ = new (std::nothrow) char*[3];
	if (!execve_argv_) {
		return -1;
	}

	std::string path = "/usr/bin/python3";

	execve_argv_[0] = new (std::nothrow) char[path.size() + 1];
	if (!execve_argv_[0]) {
		delete[] execve_argv_;
		return -1;
	}
	std::strcpy(execve_argv_[0], path.c_str());

	std::string script = getMetaVariable("SCRIPT_NAME");
	script = "/home/ubuntu2204/Documents/prg/42tokyo/webserv" + script;
	execve_argv_[1] = new (std::nothrow) char[script.size() + 1];
	if (!execve_argv_[1]) {
		delete[] execve_argv_[0];
		delete[] execve_argv_;
		return -1;
	}
	std::strcpy(execve_argv_[1], script.c_str());

	execve_argv_[2] = NULL;

	return 0;
}

int CgiRequestContext::setupCgiMetaVariables() {
	MetaVariableFunc functions[] = {
		&CgiRequestContext::authType,
		&CgiRequestContext::contentLength,
		&CgiRequestContext::contentType,
		&CgiRequestContext::gatewayInterface,
		&CgiRequestContext::pathInfo,
		&CgiRequestContext::pathTranslated,
		&CgiRequestContext::queryString,
		&CgiRequestContext::remoteAddr,
		&CgiRequestContext::remoteHost,
		&CgiRequestContext::remoteIdet,
		&CgiRequestContext::remoteUser,
		&CgiRequestContext::requestMethod,
		&CgiRequestContext::scriptName,
		&CgiRequestContext::serverName,
		&CgiRequestContext::serverPort,
		&CgiRequestContext::serverProtocol,
		&CgiRequestContext::serverSoftware,
	};

	int funcSize = sizeof(functions) / sizeof(MetaVariableFunc);

	for (int i = 0; i < funcSize; ++i) {
		(this->*functions[i])();
	}

	return 0;
}

int CgiRequestContext::createEnviron() {
	size_t size = meta_variables_.size();

	environ_ = new (std::nothrow) char*[size + 1];
	if (!environ_) {
		return -1;
	}

	size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it = meta_variables_.begin();
		 it != meta_variables_.end();
		 ++it) {
		std::string env = it->first + "=" + it->second;
		environ_[i] = new (std::nothrow) char[env.size() + 1];
		if (!environ_[i]) {
			for (size_t j = 0; j < i; ++j) {
				delete[] environ_[j];
			}
			delete[] environ_;
			environ_ = NULL;
			return -1;
		}
		std::strcpy(environ_[i], env.c_str());
		++i;
	}
	environ_[i] = NULL;
	return 0;
}

char** CgiRequestContext::getExecveArgv() const {
	return execve_argv_;
}

char** CgiRequestContext::getCgiEnviron() const {
	return environ_;
}

HttpRequest const& CgiRequestContext::getHttpRequest() const {
	return request_;
}

std::string const CgiRequestContext::getMetaVariable(std::string const& key) const {
	std::map<std::string, std::string>::const_iterator it = meta_variables_.find(key);
	if (it != meta_variables_.end()) {
		return it->second;
	}
	return std::string("");
}

std::ostream& operator<<(std::ostream& out, const CgiRequestContext& cgi_meta_variables) {
	char** iterator = cgi_meta_variables.getCgiEnviron();
	out << "CgiMetaVariables: " << std::endl;

	for (size_t i = 0; iterator[i]; ++i) {
		out << iterator[i] << std::endl;
	}
	return out;
}

} // namespace server
