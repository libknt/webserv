#include "http_request.hpp"
#include "parse_http_request.hpp"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(int argc, char* argv[]) {
	server::ParseHttpRequest parse_http_request;
	std::vector<int> fd(argc);
	bool is_all_read = false;
	char buffer[BUFFER_SIZE];
	sockaddr_in server_address;
	sockaddr_in client_address;

	for (int i = 1; i < argc; i++) {
		fd[i] = open(argv[i], O_RDWR);
		parse_http_request.addAcceptClientInfo(fd[i], client_address, server_address);
	}
	while (!is_all_read) {
		is_all_read = true;
		for (int i = 1; i < argc; i++) {
			memset(buffer, '\0', BUFFER_SIZE);
			int size = read(fd[i], buffer, BUFFER_SIZE);
			if (0 < size) {
				is_all_read = false;
				parse_http_request.handleBuffer(fd[i], buffer);
			}
		}
	}
	for (int i = 1; i < argc; i++) {
		if (0 < fd[i]) {
			server::HttpRequest & request = parse_http_request.getHttpRequest(fd[i]);
			if (request.getStatus() == server::http_request_status::FINISHED)
				std::cerr << "TEST" << argv[i] << ": OK" << std::endl;
			else
				std::cerr << "TEST" << argv[i] << ": NG" << request.getStatus()
						  << std::endl;
		}
	}
}
