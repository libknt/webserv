#include "cgi_meta_variables.hpp"

namespace server {

CgiMetaVariables::CgiMetaVariables(HttpRequest const& request)
	: request_(request)
	, meta_variables_(std::map<std::string, std::string>())
	, environ_(NULL) {}

CgiMetaVariables::CgiMetaVariables(CgiMetaVariables const& other)
	: request_(other.request_)
	, meta_variables_(other.meta_variables_)
	, environ_(other.environ_) {}

CgiMetaVariables& CgiMetaVariables::operator=(CgiMetaVariables const& other) {
	if (this != &other) {
		meta_variables_ = meta_variables_;
		// todo deepcopy
		environ_ = other.environ_;
	}
	return *this;
}

CgiMetaVariables::~CgiMetaVariables() {
	if (environ_) {
		delete[] environ_;
	}
}

void CgiMetaVariables::setMetaVariables(std::string const& key, std::string const& value) {
	meta_variables_.insert(std::make_pair(key, value));
}

} // namespace server