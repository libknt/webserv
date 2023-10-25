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
	:request_(other.request_) 
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
	meta_variables_.insert(std::make_pair("AUTH_TYPE", auth_type ));
	return 0;
}

int CgiMetaVariables::content_length() {
	meta_variables_.insert(std::make_pair("CONTENT_LENGTH", request_.getHeaderValue("CONTENT_LENGTH")));
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
	meta_variables_.insert(std::make_pair("REMOTE_ADDR", ""));
	return 0;
}

int CgiMetaVariables::remote_host() {
	meta_variables_.insert(std::make_pair("REMOTE_HOST", ""));
	return 0;
}

int CgiMetaVariables::remote_idet() {
	meta_variables_.insert(std::make_pair("REMOTE_IDENT", ""));
	return 0;
}

int CgiMetaVariables::remote_user() {
	meta_variables_.insert(std::make_pair("REMOTE_USER", ""));
	return 0;
}
int CgiMetaVariables::request_method() {
	meta_variables_.insert(std::make_pair("REQUEST_METHOD", ""));
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