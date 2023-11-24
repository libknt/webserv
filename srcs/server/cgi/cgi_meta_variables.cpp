#include "cgi_meta_variables.hpp"

CgiMetaVariables::CgiMetaVariables()
	: meta_variables_(std::map<std::string, std::string>()) {}

CgiMetaVariables::CgiMetaVariables(CgiMetaVariables const& other)
	: meta_variables_(other.meta_variables_) {}

CgiMetaVariables& CgiMetaVariables::operator=(CgiMetaVariables const& other) {
	if (this != &other) {
		meta_variables_ = meta_variables_;
	}
	return *this;
}

CgiMetaVariables::~CgiMetaVariables() {}

void CgiMetaVariables::setMetaVariables(std::string const& key, std::string const& value) {
	meta_variables_.insert(std::make_pair(key, value));
}
