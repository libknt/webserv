#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>
#include <sys/stat.h>

namespace server {

class Utils {
private:
	Utils();
	Utils(const Utils& other);
	Utils& operator=(const Utils& other);
	~Utils();

public:
	static bool isFilePresent(const std::string& path);
	static std::string getUriExtension(const std::string& uri);
	template<typename T>
	static std::string toString(const T& value);
};

#include "utils.tpp"

} // namespace server

#endif // UTILS_HPP