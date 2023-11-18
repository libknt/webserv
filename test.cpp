#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		std::string uri(argv[i]);
		std::string::size_type query_index = uri.find("?");
		if (query_index != std::string::npos) {
			std::cout << uri.substr(0, query_index) << std::endl;
			continue;
		}
		std::cout << uri << std::endl;
	}
}
