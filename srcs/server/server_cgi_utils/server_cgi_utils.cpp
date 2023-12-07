#include "server_cgi_utils.hpp"

namespace server_cgi_utils {

static std::string authTye(std::string const& value) {
	std::string auth_type(value);
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	return auth_type;
}

static std::string pathInfo(const std::string& value) {
	const std::string path(value);
	const std::string::size_type extension_position = path.find(".");

	if (extension_position != std::string::npos) {
		const std::string::size_type slash_position = path.find("/", extension_position + 1);
		if (slash_position != std::string::npos) {
			const std::string path_info = path.substr(slash_position);
			return path_info;
		}
	}
	return std::string("");
}

static std::string pathTranslated(std::string const& root, std::string const& path_info) {
	std::string path_translated = root + path_info;
	return path_translated;
}

static std::string remoteAddr(sockaddr_in const& client_address) {
	uint32_t addr = ntohl(client_address.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	return ip_stream.str();
}

static std::string remoteHost() {
	// TODO 取得する関数が、使用可能関数にない
	return std::string("");
}

static std::string remoteIdet() {
	// TODO 推奨されてないので空
	return std::string("");
}

static inline bool isBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::string base64Decode(std::string const& encoded_string) {
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

static std::string remoteUser(std::string const& value) {
	std::string remote_user(value);
	if (remote_user.compare("") != 0) {
		remote_user = remote_user.substr(remote_user.find(' ') + 1);
		remote_user = base64Decode(remote_user);
		remote_user = remote_user.substr(0, remote_user.find(':'));
	}
	return remote_user;
}

static std::string scriptName(const std::string& value, const std::string& path_info) {
	std::string script_name(value);
	if (!path_info.empty()) {
		script_name = script_name.erase(script_name.size() - path_info.size(), path_info.size());
	}
	return script_name;
}

static std::string serverName(sockaddr_in const& server_address) {

	uint32_t addr = ntohl(server_address.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	return ip_stream.str();
}

static std::string serverPort(sockaddr_in const& server_address) {
	int port = ntohs(server_address.sin_port);
	std::stringstream port_stream;
	port_stream << port;
	return port_stream.str();
}

void createCgiMetaVariables(std::map<std::string, std::string>& meta_variables,
	server::HttpRequest const& request,
	sockaddr_in const& client_address,
	sockaddr_in const& server_address,
	std::string const& root_path) {

	meta_variables.clear();
	meta_variables.insert(
		std::make_pair("AUTH_TYPE", authTye(request.getHeaderValue("authorization"))));
	meta_variables.insert(
		std::make_pair("CONTENT_LENGTH", request.getHeaderValue("content-length")));
	meta_variables.insert(std::make_pair("CONTENT_TYPE", request.getHeaderValue("content-type")));
	meta_variables.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	meta_variables.insert(std::make_pair("PATH_INFO", pathInfo(request.getUriPath())));
	meta_variables.insert(std::make_pair(
		"PATH_TRANSLATED", pathTranslated(root_path, meta_variables.find("PATH_INFO")->second)));
	meta_variables.insert(std::make_pair("QUERY_STRING", request.getUriQuery()));
	meta_variables.insert(std::make_pair("REMOTE_ADDR", remoteAddr(client_address)));
	meta_variables.insert(std::make_pair("REMOTE_HOST", remoteHost()));
	meta_variables.insert(std::make_pair("REMOTE_IDENT", remoteIdet()));
	meta_variables.insert(
		std::make_pair("REMOTE_USER", remoteUser(request.getHeaderValue("authorization"))));
	meta_variables.insert(std::make_pair("REQUEST_METHOD", request.getMethod()));
	meta_variables.insert(std::make_pair(
		"SCRIPT_NAME", scriptName(request.getUriPath(), meta_variables.find("PATH_INFO")->second)));
	meta_variables.insert(std::make_pair("SERVER_NAME", serverName(server_address)));
	meta_variables.insert(std::make_pair("SERVER_PORT", serverPort(server_address)));
	meta_variables.insert(std::make_pair("SERVER_PROTOCOL", request.getVersion()));
	meta_variables.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
}

} // namespace server_cgi_utils
