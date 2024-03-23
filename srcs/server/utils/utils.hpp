#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>
#include <sys/stat.h>

class Utils {
private:
	Utils();
	Utils(const Utils& other);
	Utils& operator=(const Utils& other);
	~Utils();

public:
	static bool fileExists(const std::string& path);
	static std::string extructUriExtension(const std::string& uri);
	template<typename T>
	static std::string toString(const T& value);
	static int toNumber(const std::string& value);
};
#include "utils.tpp"

#endif // UTILS_HPP