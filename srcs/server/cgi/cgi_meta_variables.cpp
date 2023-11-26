#include "cgi_meta_variables.hpp"

namespace server {

char** DeepCopyCharPointerArray(char** source);

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
		environ_ = DeepCopyCharPointerArray(other.environ_);
	}
	return *this;
}

char** DeepCopyCharPointerArray(char** source) {
	if (!source) {
		return NULL;
	}

	size_t count = 0;
	while (source[count]) {
		++count;
	}

	char** destination = new (std::nothrow) char*[count + 1];
	if (!destination) {
		return NULL;
	}

	for (size_t i = 0; i < count; ++i) {
		size_t length = std::strlen(source[i]) + 1;
		destination[i] = new (std::nothrow) char[length];
		if (!destination[i]) {
			for (size_t j = 0; j < i; ++j) {
				delete[] destination[j];
			}
			delete[] destination;
			return NULL;
		}
		std::strcpy(destination[i], source[i]);
	}

	destination[count] = NULL;
	return destination;
}

CgiMetaVariables::~CgiMetaVariables() {
	if (environ_) {
		for (size_t i = 0; environ_[i]; ++i) {
			delete[] environ_[i];
		}
		delete[] environ_;
	}
}

void CgiMetaVariables::setMetaVariables(std::string const& key, std::string const& value) {
	meta_variables_.insert(std::make_pair(key, value));
}

int CgiMetaVariables::authType() {
	std::string auth_type = request_.getHeaderValue("Authorization");
	if (auth_type.compare("") != 0) {
		auth_type = auth_type.substr(0, auth_type.find(' '));
	}
	setMetaVariables("AUTH_TYPE", auth_type);
	return 0;
}

} // namespace server