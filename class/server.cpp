#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "TcpServer.hpp"

int main() {
	http::TcpServer server(SERVER_ADDR, SERVER_PORT);

	if (server.initialize() < 0) {
		std::cerr << "Failed to initialize server." << std::endl;
		return 1;
	}
	std::cout << "Server started. Waiting for connections..." << std::endl;
	int status = server.start();
	return status;
}
