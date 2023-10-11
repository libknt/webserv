#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace http {

class ServerSocket {
private:
	const char* server_addr_;
	const int port_;
	int listen_sd_;
	int backlog_;
	struct sockaddr_in addr;

	ServerSocket();
	int socket();
	int setsockopt();
	int nonBlock();
	int setSockaddr();
	int bind();
	int listen();

public:
	ServerSocket(const char* server_addr, int port);
	~ServerSocket();
	int initialize();
	const char* getServerAddr_() const;
	int getPort_() const;
	int getListenSd() const;
};
}

#endif
