#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

// シバン（shebang）
// #!/usr/local/bin/python3
int shebang(std::string file, std::string& path) {
	std::ifstream infile(file.c_str());
	if (!infile) {
		std::cout << "file open error: " << file << std::endl;
		return -1;
	}
	std::string line;
	std::getline(infile, line);
	path.clear();
	if (strncmp(line.c_str(), "#!", 2) == 0) {
		line = line.substr(2);
		int access_flag = access(line.c_str(), X_OK);
		if (access_flag == 0) {
			path = line;
		}
	}
	return 0;
}

// int main() {
// 	std::string path;
// 	shebang(std::string("/Users/kyoda/Desktop/webserv/cgi-bin/hello.py"), path);
// 	std::cout << path << std::endl;
// }