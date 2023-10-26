#include <cstring>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <path to request file>" << std::endl;
		return 1;
	}

	// ファイルからリクエストを読み込む
	std::ifstream infile(argv[1]);
	if (!infile.is_open()) {
		std::cerr << "ERROR opening file: " << argv[1] << std::endl;
		return 1;
	}

	std::stringstream bufferss;
	bufferss << infile.rdbuf();
	std::string request = bufferss.str();
	infile.close();

	// サーバの情報のセットアップ
	struct hostent* server;
	struct sockaddr_in serv_addr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		std::cerr << "ERROR opening socket" << std::endl;
		return 1;
	}

	server = gethostbyname("127.0.0.1");
	if (server == NULL) {
		std::cerr << "ERROR, no such host" << std::endl;
		return 1;
	}

	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	serv_addr.sin_port = htons(8080);

	// サーバへの接続
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "ERROR connecting" << std::endl;
		return 1;
	}

	// リクエストの送信
	ssize_t n = send(sockfd, request.c_str(), request.size(), 0);
	if (n < 0) {
		std::cerr << "ERROR writing to socket" << std::endl;
		return 1;
	}

	// レスポンスの受信
	char buffer[4096];
	n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
	if (n < 0) {
		std::cerr << "ERROR reading from socket" << std::endl;
		return 1;
	}

	buffer[n] = '\0';
	std::cout << "Received response:\n" << buffer << std::endl;

	close(sockfd);
	return 0;
}
