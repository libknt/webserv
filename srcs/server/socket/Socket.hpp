#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace server {

class Socket {
private:
	const char* server_addr_;
	int port_;
	int listen_sd_;
	int backlog_; // macOs sysctl kern.ipc.somaxconn
	struct sockaddr_in addr_;

	Socket();
	int socket();
	int setsockopt();
	int nonBlock();
	int setSockaddr();
	int bind();
	int listen();
	bool isValid();

public:
	Socket(const char* server_addr, int port);
	Socket(const char* server_addr, int port, int back_log);
	Socket(const Socket& r);
	Socket& operator=(const Socket& other);
	virtual ~Socket();
	int initialize();
	const char* getServerAddr() const;
	int getPort() const;
	int getListenSd() const;
};
}

#endif
