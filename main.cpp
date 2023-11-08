#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	// ファイルからデータを読み込む
	std::ifstream infile("send.txt");
	std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}

	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Error connecting" << std::endl;
		return 1;
	}

	send(sockfd, content.c_str(), content.size(), 0);

	char buffer[4096];
	int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		return 1;
	}

	std::ofstream outfile("recv.txt");
	outfile.write(buffer, bytes_received);

	close(sockfd);
	return 0;
}
