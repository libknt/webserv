#include "http_request.hpp"
#include "http_request_parser.hpp"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <vector>

int main(int argc, char* argv[]) {
	server::HttpRequestParser http_request_parser;
	std::vector<int> fd(argc);
	bool is_all_read = false;
	char buffer[server::BUFFER_SIZE];
	sockaddr_in server_address;
	sockaddr_in client_address;

	std::cout << "server::BUFFER_SIZE: " << server::BUFFER_SIZE << std::endl;
	for (int i = 1; i < argc; i++) {
		fd[i] = open(argv[i], O_RDWR);
		http_request_parser.addAcceptClientInfo(fd[i], client_address, server_address);
	}
	while (!is_all_read) {
		is_all_read = true;
		for (int i = 1; i < argc; i++) {
			memset(buffer, '\0', server::BUFFER_SIZE);
			int size = read(fd[i], buffer, server::BUFFER_SIZE - 1);
			if (0 < size) {
				is_all_read = false;
				http_request_parser.handleBuffer(fd[i], buffer);
			}
		}
	}
	for (int i = 1; i < argc; i++) {
		if (0 < fd[i]) {
			server::HttpRequest const request(http_request_parser.getRequest(fd[i]));
			if ((std::string(argv[i]).find("success") != std::string::npos &&
					request.getStatus() == server::http_request_status::FINISHED) ||
				(std::string(argv[i]).find("failure") != std::string::npos &&
					request.getStatus() == server::http_request_status::ERROR)) {
				std::cerr << "TEST" << argv[i] << ": OK" << std::endl;
				std::cout << request << std::endl;
			} else {
				std::cerr << "TEST" << argv[i] << ": NG" << request.getStatus() << std::endl;
				std::cout << request << std::endl;
				exit(1);
			}
			close(fd[i]);
		}
	}
}
