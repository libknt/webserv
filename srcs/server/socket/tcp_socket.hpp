#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace server {

class TcpSocket {
private:
	std::string ip_address_;
	std::string port_;
	int listen_sd_;
	int backlog_; // macOs sysctl kern.ipc.somaxconn
	struct sockaddr_in socket_address_;

	static const int min_port_ = 0;
	static const int max_port_ = 65535;

	TcpSocket();
	int createTcpSocket();
	int setSocketOption();
	int setSocketToNonBlocking();
	int configureSocketAddress();
	int bindAddressToSocket();
	int startListening();
	bool isSocketConfigValid();

public:
	TcpSocket(std::string server_address, std::string port);
	TcpSocket(const TcpSocket& other);
	TcpSocket& operator=(const TcpSocket& other);
	virtual ~TcpSocket();
	int setupSocketForListening();
	int getListenSd() const;
};
}

#endif
