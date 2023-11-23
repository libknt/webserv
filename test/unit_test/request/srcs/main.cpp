#include "webserv.hpp"
#include "http_request.hpp"
#include "http_request_parser.hpp"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <vector>

// namespace server {
int main(int argc, char* argv[]) {
	server::HttpRequest requests[argc + 1];
	std::vector<int> fd(argc);

	bool is_all_read = false;
	char buffer[BUFFER_SIZE];

	std::cout << "BUFFER_SIZE: " << BUFFER_SIZE << std::endl;
	for (int i = 1; i < argc; i++) {
		fd[i] = open(argv[i], O_RDWR);
	}
	while (!is_all_read) {
		is_all_read = true;
		for (int i = 1; i < argc; i++) {
			memset(buffer, '\0', BUFFER_SIZE);
			int size = read(fd[i], buffer, BUFFER_SIZE - 1);
			if (0 < size) {
				is_all_read = false;
				server::HttpRequestParser::parse(requests[i], buffer);
			}
		}
	}
	for (int i = 1; i < argc; i++) {
		if (0 < fd[i]) {
			if ((std::string(argv[i]).find("success") != std::string::npos &&
					requests[i].getStatus() == server::http_request_status::FINISHED) ||
				(std::string(argv[i]).find("failure") != std::string::npos &&
					requests[i].getStatus() == server::http_request_status::ERROR)) {
				std::cerr << "TEST" << argv[i] << ": OK" << std::endl;
				std::cout << requests[i] << std::endl;
			} else {
				std::cerr << "TEST" << argv[i] << ": NG" << requests[i].getStatus() << std::endl;
				std::cout << requests[i] << std::endl;
				exit(1);
			}
			close(fd[i]);
		}
	}
	return (0);
}
//}