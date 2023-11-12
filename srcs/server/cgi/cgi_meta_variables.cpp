#include "cgi_meta_variables.hpp"

namespace server {

CgiMetaVariables::CgiMetaVariables(const HttpRequest& request)
	: request_(request)
	, meta_variables_() {
	std::cout << "CgiMetaVariables::CgiMetaVariables(const HttpRequest& request)" << std::endl;
	// metaFuncArray.push_back(&CgiMetaVariables::authType);
}

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
	std::cout << "CgiMetaVariables::setup()" << std::endl;
	std::string meta_variable;
	for (std::vector<MetaFuncPtr>::iterator it = metaFuncArray.begin(); it != metaFuncArray.end();
		 ++it) {
		if ((this->**it)() < 0)
			return -1;
	}
	return 0;
}

void CgiMetaVariables::setMetaVariables(std::string key, std::string value) {
	meta_variables_.insert(std::make_pair(key, value));
}

} // namespace server