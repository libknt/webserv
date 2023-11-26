#include "cgi_meta_variables.hpp"

namespace server {

char** DeepCopyCharPointerArray(char** source);

CgiMetaVariables::CgiMetaVariables(HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address)
	: request_(request)
	, meta_variables_(std::map<std::string, std::string>())
	, client_address_(client_address)
	, server_address_(server_address)
	, environ_(NULL) {}

CgiMetaVariables::CgiMetaVariables(CgiMetaVariables const& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_)
	, client_address_(other.client_address_)
	, server_address_(other.server_address_)
	, environ_(other.environ_) {}

CgiMetaVariables& CgiMetaVariables::operator=(CgiMetaVariables const& other) {
	if (this != &other) {
		meta_variables_ = meta_variables_;
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

CgiMetaVariables::~CgiMetaVariables() {
	if (environ_) {
		for (size_t i = 0; environ_[i]; ++i) {
			delete[] environ_[i];
		}
		delete[] environ_;
	}
}

void CgiMetaVariables::setMetaVariables(std::string const& key, std::string const& value) {
	meta_variables_.insert(std::make_pair(key, value));
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
	uint32_t addr = ntohl(client_address_.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	setMetaVariables("REMOTE_ADDR", ip_stream.str());
	return 0;
}

int CgiMetaVariables::remoteHost() {
	// TODO 取得する関数が、使用可能関数にない
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

int CgiMetaVariables::serverName() {

	uint32_t addr = ntohl(server_address_.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	setMetaVariables("SERVER_NAME", ip_stream.str());
	return 0;
}

} // namespace server