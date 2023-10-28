#include "ClientSocket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int rc;
	struct sockaddr_in server_addr;
	char sendBuffer[1024];
	char recvBuffer[1024];

	int flag = 0;
	if (argc == 4) {
		if (strcmp(argv[3], "--echo-test") == 0) {
			flag = 1;
		} else if (strcmp(argv[3], "--connect") == 0) {
			flag = 2;
		}
	}
	try {
		http::ClientSocket client_socket(argv[1], std::stoi(argv[2]));
		if (client_socket.initialize() < 0)
			return -1;
		if (flag == 2)
			return 0;

		printf("Connected to server. Enter a message to send:\n");
		while (1) {
			fgets(sendBuffer, sizeof(sendBuffer), stdin);

			rc = send(client_socket.getClientSocket(), sendBuffer, std::strlen(sendBuffer), 0);
			if (rc < 0) {
				perror("send() failed");
				exit(-1);
			}

			rc = recv(client_socket.getClientSocket(), recvBuffer, sizeof(recvBuffer), 0);
			if (rc < 0) {
				perror("recv() failed");
				exit(-1);
			} else if (rc == 0) {
				printf("Server closed the connection.\n");
				break;
			}

			recvBuffer[rc] = '\0';
			printf("\033[34mReceived from server: %s\033[0m", recvBuffer);
			fflush(stdout);
			if (flag == 1) {
				if (strcmp(sendBuffer, recvBuffer) == 0) {
					std::cout << "echo success!!!!!!!!!" << std::endl;
					return 0;
				}
			}
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	return 0;
}
