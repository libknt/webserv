#ifndef CGI_MRTA_VARIABLES_HPP
#define CGI_MRTA_VARIABLES_HPP

#include "http_request.hpp"
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <new>
#include <sstream>
#include <string>
#include <vector>

class HttpRequest;
enum URL_META_VARIABLES {
	SCRIPT_NAME,
	PATH_INFO,
	PATH_TRANSLATED,
	QUERY_STRING,
};

namespace server {

class CgiMetaVariables {
private:
	HttpRequest& request_;
	std::map<std::string, std::string> meta_variables_;
	typedef int (CgiMetaVariables::*MetaFuncPtr)();
	std::vector<MetaFuncPtr> metaFuncArray;
	char** exec_environ_;
	int auth_type();
	int content_length();
	int content_type();
	int gateway_interface();
	int path_info();
	int path_translated();
	int query_string();
	int remote_addr();
	int remote_host();
	int remote_idet();
	int remote_user();
	int request_method();
	int script_name();
	int server_name();
	int server_port();
	int server_protocol();
	int server_software();
	int set_meta_variables();

	static inline bool is_base64(unsigned char c);
	static std::string base64_decode(std::string const& encoded_string);
	int url_parse(std::string request_path, std::string& parsed_line, URL_META_VARIABLES what);
	std::string extract_script_name(std::string& path);
	std::string extract_query(std::string& path);
	std::string extract_path_info(std::string& path);
	std::string extract_after_cgi_bin(std::string& request_path);

	explicit CgiMetaVariables();

public:
	explicit CgiMetaVariables(HttpRequest& request);
	explicit CgiMetaVariables(const CgiMetaVariables& other);
	CgiMetaVariables& operator=(const CgiMetaVariables& other);
	virtual ~CgiMetaVariables();
	int create_meta_variables();
	int unset_meta_variables();
	void get_meta();
};

}
#endif