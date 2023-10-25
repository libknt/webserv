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
	, exec_environ_(NULL)
	{
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
	// meta_variables_.insert(std::make_pair("PATH_INFO", ""));
	return 0;
}

int CgiMetaVariables::path_translated() {
	// meta_variables_.insert(std::make_pair("PATH_TRANSLATED", ""));
	return 0;
}

int CgiMetaVariables::query_string() {
	// meta_variables_.insert(std::make_pair("QUERY_STRING", ""));
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
static const std::string base64_chars ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
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
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
  }

  return ret;
}
int CgiMetaVariables::remote_user() {
	std::string remote_user = request_.getHeaderValue("Authorization");
	if (remote_user.compare("") != 0) {
		remote_user = remote_user.substr(remote_user.find(' ') + 1);
		remote_user = base64_decode(remote_user);
		remote_user = remote_user.substr(0,remote_user.find(':'));
	}
	meta_variables_.insert(std::make_pair("REMOTE_USER", remote_user));
	return 0;
}

int CgiMetaVariables::request_method() {
	std::string method;
	switch (request_.get_http_method())
	{
	case http_method::GET:
		method = "GET";
		break;
	case http_method::POST:
		method = "POST";
		break;
	case http_method::DELETE:
		method = "DELETE";
		break;
	default:
		return -1;
	}
	meta_variables_.insert(std::make_pair("REQUEST_METHOD", method));
	return 0;
}

int CgiMetaVariables::script_name() {
	meta_variables_.insert(std::make_pair("SCRIPT_NAME", ""));
	return 0;
}

int CgiMetaVariables::server_name() {
	meta_variables_.insert(std::make_pair("SERVER_NAME", ""));
	return 0;
}
int CgiMetaVariables::server_port() {
	meta_variables_.insert(std::make_pair("SERVER_PORT", ""));
	return 0;
}
int CgiMetaVariables::server_protocol() {
	meta_variables_.insert(std::make_pair("SERVER_PROTOCOL", ""));
	return 0;
}

int CgiMetaVariables::server_software() {
	meta_variables_.insert(std::make_pair("SERVER_SOFTWARE", ""));
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
}
