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