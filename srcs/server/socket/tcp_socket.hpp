#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

namespace server {

class TcpSocket {
private:
	std::string ip_address_;
	std::string port_;
	int listen_sd_;
	int backlog_;
	struct sockaddr_in socket_address_;

	TcpSocket();
	bool isSocketConfigValid();
	int createTcpSocket();
	int setSocketOption();
	int setSocketToNonBlocking();
	int configureSocketAddress();
	int bindAddressToSocket();
	int startListening();

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
