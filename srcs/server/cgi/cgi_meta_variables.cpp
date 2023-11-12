#include "cgi_meta_variables.hpp"

namespace server {

cgiMetaVariables::~cgiMetaVariables() {}

cgiMetaVariables::cgiMetaVariables(const cgiMetaVariables& other)
	: meta_variables_(other.meta_variables_) {}

cgiMetaVariables& cgiMetaVariables::operator=(const cgiMetaVariables& other) {
	if (this != &other) {
	}
	return *this;
}

} // namespace server