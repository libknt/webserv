#include "cgi_meta_variables.hpp"

namespace server {
// CgiMetaVariables::CgiMetaVariables()
// 	: exec_environ_(NULL) {
// 	metaFuncArray.push_back(&CgiMetaVariables::auth_type);
// 	metaFuncArray.push_back(&CgiMetaVariables::content_length);
// 	metaFuncArray.push_back(&CgiMetaVariables::content_type);
// 	metaFuncArray.push_back(&CgiMetaVariables::gateway_interface);
// 	metaFuncArray.push_back(&CgiMetaVariables::path_info);
// 	metaFuncArray.push_back(&CgiMetaVariables::path_translated);
// 	metaFuncArray.push_back(&CgiMetaVariables::query_string);
// 	metaFuncArray.push_back(&CgiMetaVariables::remote_addr);
// 	metaFuncArray.push_back(&CgiMetaVariables::remote_host);
// 	metaFuncArray.push_back(&CgiMetaVariables::remote_user);
// 	metaFuncArray.push_back(&CgiMetaVariables::request_method);
// 	metaFuncArray.push_back(&CgiMetaVariables::script_name);
// 	metaFuncArray.push_back(&CgiMetaVariables::server_name);
// 	metaFuncArray.push_back(&CgiMetaVariables::server_port);
// 	metaFuncArray.push_back(&CgiMetaVariables::server_protocol);
// 	metaFuncArray.push_back(&CgiMetaVariables::server_software);
// }

CgiMetaVariables::CgiMetaVariables(HttpRequest& request)
	: request_(request)
	, exec_environ_(NULL) {
	metaFuncArray.push_back(&CgiMetaVariables::auth_type);
	metaFuncArray.push_back(&CgiMetaVariables::content_length);
	metaFuncArray.push_back(&CgiMetaVariables::content_type);
	metaFuncArray.push_back(&CgiMetaVariables::gateway_interface);
	metaFuncArray.push_back(&CgiMetaVariables::path_info);
	metaFuncArray.push_back(&CgiMetaVariables::path_translated);
	metaFuncArray.push_back(&CgiMetaVariables::query_string);
	metaFuncArray.push_back(&CgiMetaVariables::remote_addr);
	metaFuncArray.push_back(&CgiMetaVariables::remote_host);
	metaFuncArray.push_back(&CgiMetaVariables::remote_user);
	metaFuncArray.push_back(&CgiMetaVariables::request_method);
	metaFuncArray.push_back(&CgiMetaVariables::script_name);
	metaFuncArray.push_back(&CgiMetaVariables::server_name);
	metaFuncArray.push_back(&CgiMetaVariables::server_port);
	metaFuncArray.push_back(&CgiMetaVariables::server_protocol);
	metaFuncArray.push_back(&CgiMetaVariables::server_software);
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

int CgiMetaVariables::auth_type() {
	std::string auth_type = request_.getHeaderValue("Authorization");
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	meta_variables_.insert(std::make_pair("AUTH_TYPE", auth_type));
	return 0;
}

int CgiMetaVariables::content_length() {
	meta_variables_.insert(
		std::make_pair("CONTENT_LENGTH", request_.getHeaderValue("CONTENT_LENGTH")));
	return 0;
}

int CgiMetaVariables::content_type() {
	meta_variables_.insert(std::make_pair("CONTENT_TYPE", request_.getHeaderValue("CONTENT_TYPE")));
	return 0;
}

int CgiMetaVariables::gateway_interface() {
	meta_variables_.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	return 0;
}

int CgiMetaVariables::path_info() {
	std::string path_info;
	url_parse(request_.get_request_path(), path_info, PATH_INFO);
	meta_variables_.insert(std::make_pair("PATH_INFO", path_info));
	return 0;
}

int CgiMetaVariables::path_translated() {
	std::string path_translated;
	url_parse(request_.get_request_path(), path_translated, PATH_TRANSLATED);
	// TODO serverのドキュメントルートに合わせる.
	// /var/www/html/additional/path/info
	meta_variables_.insert(std::make_pair("PATH_TRANSLATED", path_translated));
	return 0;
}

int CgiMetaVariables::query_string() {
	std::string query;
	url_parse(request_.get_request_path(), query, QUERY_STRING);
	meta_variables_.insert(std::make_pair("QUERY_STRING", query));
	return 0;
}

int CgiMetaVariables::remote_addr() {
	sockaddr_in client_addr = request_.get_client_addr();
	uint32_t addr = ntohl(client_addr.sin_addr.s_addr);
	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
	meta_variables_.insert(std::make_pair("REMOTE_ADDR", ip_stream.str()));
	return 0;
}

int CgiMetaVariables::remote_host() {
	// 取得する関数が、使用可能関数にないと思う
	meta_variables_.insert(std::make_pair("REMOTE_HOST", ""));
	return 0;
}

int CgiMetaVariables::remote_idet() {
	// 推奨されてないので空
	meta_variables_.insert(std::make_pair("REMOTE_IDENT", ""));
	return 0;
}

inline bool CgiMetaVariables::is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CgiMetaVariables::base64_decode(std::string const& encoded_string) {
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
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
int CgiMetaVariables::remote_user() {
	std::string remote_user = request_.getHeaderValue("Authorization");
	if (remote_user.compare("") != 0) {
		remote_user = remote_user.substr(remote_user.find(' ') + 1);
		remote_user = base64_decode(remote_user);
		remote_user = remote_user.substr(0, remote_user.find(':'));
	}
	meta_variables_.insert(std::make_pair("REMOTE_USER", remote_user));
	return 0;
}

int CgiMetaVariables::request_method() {
	std::string method = request_.get_http_method();
	meta_variables_.insert(std::make_pair("REQUEST_METHOD", method));
	return 0;
}

int CgiMetaVariables::script_name() {
	std::string script_name;
	url_parse(request_.get_request_path(), script_name, SCRIPT_NAME);
	meta_variables_.insert(std::make_pair("SCRIPT_NAME", script_name));
	return 0;
}

int CgiMetaVariables::server_name() {
	sockaddr_in server_addr = request_.get_server_addr();

	uint32_t addr = ntohl(server_addr.sin_addr.s_addr);

	std::ostringstream ip_stream;
	ip_stream << ((addr >> 24) & 0xFF) << "." << ((addr >> 16) & 0xFF) << "."
			  << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);

	meta_variables_.insert(std::make_pair("SERVER_NAME", ip_stream.str()));
	return 0;
}

int CgiMetaVariables::server_port() {
	sockaddr_in server_addr = request_.get_server_addr();
	int port = ntohs(server_addr.sin_port);
	std::stringstream ss;
	ss << port;
	std::string port_str = ss.str();
	meta_variables_.insert(std::make_pair("SERVER_PORT", port_str));
	return 0;
}

int CgiMetaVariables::server_protocol() {
	std::string protocol = request_.get_server_protocol();
	meta_variables_.insert(std::make_pair("SERVER_PROTOCOL", protocol));
	return 0;
}

int CgiMetaVariables::server_software() {
	meta_variables_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	return 0;
}

int CgiMetaVariables::create_meta_variables() {
	for (std::vector<MetaFuncPtr>::iterator it = metaFuncArray.begin(); it != metaFuncArray.end();
		 ++it) {
		if ((this->**it)() < 0)
			return -1;
	}
	set_meta_variables();
	return 0;
}

int CgiMetaVariables::set_meta_variables() {
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

int CgiMetaVariables::unset_meta_variables() {
	return 0;
}

void CgiMetaVariables::get_meta() {
	for (int i = 0;; ++i) {
		if (exec_environ_[i] == NULL)
			break;
		std::cout << exec_environ_[i] << std::endl;
	}
}

int CgiMetaVariables::url_parse(std::string request_path,
	std::string& parsed_line,
	URL_META_VARIABLES what) {
	std::cout << "path++++++++++++++: " << request_path << std::endl;

	std::string query;
	std::string path_info;
	size_t pos = request_path.find("/cgi-bin/");
	if (pos == std::string::npos) {
		std::cerr << "not found request_path[cgi-bin]" << std::endl;
		return -1;
	}
	request_path = request_path.substr(pos);
	pos = request_path.find('?');
	if (pos == std::string::npos) {
		query = std::string("");
	} else {
		query = request_path.substr(pos + 1);
		request_path = request_path.substr(0, pos);
	}

	pos = request_path.find('.');
	if (pos == std::string::npos) {
		std::cerr << "not found [cgi-script]" << std::endl;
		return -1;
	} else {
		size_t path_info_pos = request_path.find('/', pos);
		if (path_info_pos != std::string::npos) {
			path_info = request_path.substr(path_info_pos);
			request_path = request_path.substr(0, path_info_pos);
		} else {
			path_info = std::string("");
		}
	}
	std::string script(request_path);

	switch (what) {
		case SCRIPT_NAME:
			parsed_line = script;
			break;
		case PATH_INFO:
			parsed_line = path_info;
			break;
		case PATH_TRANSLATED:
			parsed_line = path_info;
			break;
		case QUERY_STRING:
			parsed_line = query;
			break;
		default:
			parsed_line = std::string("");
	}

	return 0;
}
}
