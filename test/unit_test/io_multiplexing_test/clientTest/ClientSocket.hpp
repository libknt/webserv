#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

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

class ClientSocket {
private:
	const char* server_address_;
	const int port_;
	int client_socket_;
	struct sockaddr_in addr_;

	ClientSocket();
	int socket();
	int set();
	int connect();

public:
	ClientSocket(const char* server_address, int port);
	~ClientSocket();
	int initialize();
	const char* getServerAddr_() const;
	int getPort_() const;
	int getClientSocket() const;
};
}

#endif