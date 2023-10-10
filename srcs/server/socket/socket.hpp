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

	static const int min_port_ = 0;
	static const int max_port_ = 65535;

	Socket();
	int socket();
	int setsockopt();
	int nonBlock();
	int setSocketAddress();
	int bind();
	int listen();
	bool isValid();

public:
	Socket(const char* server_addr, int port);
	Socket(const char* server_addr, int port, int backlog);
	Socket(const Socket& other);
	Socket& operator=(const Socket& other);
	virtual ~Socket();
	int initialize();
	const char* getServerAddr() const;
	int getPort() const;
	int getListenSd() const;
};
}

#endif
