#include <iostream>
#include <map>
#include <string>

std::string getFileExtension(const std::string& filename) {
	size_t pos = filename.find_last_of('.');
	if (pos != std::string::npos) {
		return filename.substr(pos);
	}
	return "";
}

int extension(std::string filename, std::string& path) {
	std::map<std::string, std::string> interpreterMap;

	interpreterMap.insert(std::make_pair(".py", "/usr/local/bin/python3"));
	interpreterMap.insert(std::make_pair(".pl", "/usr/bin/perl"));
	interpreterMap.insert(std::make_pair(".sh", "/bin/bash"));
	interpreterMap.insert(std::make_pair(".rb", "/usr/bin/ruby"));

	std::string extension = getFileExtension(filename);
	path.clear();
	for (std::map<std::string, std::string>::iterator it = interpreterMap.begin();
		 it != interpreterMap.end();
		 ++it) {
		if (it->first == extension) {
			path = it->second;
			break;
		}
	}
	return 0;
}

// int main() {
// 	std::string path;
// 	extension(std::string("/Users/kyoda/Desktop/webserv/cgi-bin/hello.py"), path);
// 	std::cout << path << std::endl;
// 	extension(std::string("/Users/kyoda/Desktop/webserv/cgi-bin/hello.pl"), path);
// 	std::cout << path << std::endl;
// 	extension(std::string("/Users/kyoda/Desktop/webserv/cgi-bin/hello.rb"), path);
// 	std::cout << path << std::endl;
// 	extension(std::string("/Users/kyoda/Desktop/webserv/cgi-bin/hello.sh"), path);
// 	std::cout << path << std::endl;
// }