#include "utils.hpp"

namespace server {

Utils::Utils() {}

Utils::~Utils() {}

Utils::Utils(const Utils& other) {
	(void)other;
}

Utils& Utils::operator=(const Utils& other) {
	(void)other;
	return *this;
}

bool Utils::fileExists(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string Utils::extructUriExtension(const std::string& uri) {
	std::string extension = "";
	size_t dot_pos = uri.find_last_of(".");
	if (dot_pos != std::string::npos) {
		extension = uri.substr(dot_pos + 1);
	}
	return extension;
}

} // namespace server
