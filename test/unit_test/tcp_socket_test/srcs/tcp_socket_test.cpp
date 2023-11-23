#include "tcp_socket.hpp"
#include <cassert>
#include <iostream>

#define RED "\e[31m"   // 赤色
#define GREEN "\e[32m" // 緑色
#define BLUE "\e[34m"  // 青色
#define RESET "\e[m"   // デフォルト色

int main() {

	std::cout << std::endl;
	{
		std::cout << GREEN << "copy constructor test: ";
		server::TcpSocket tcp_socket("127.0.0.1", "8080");
		server::TcpSocket tcp_socket_copy(tcp_socket);
		assert(tcp_socket.getIpAddress() == tcp_socket_copy.getIpAddress());
		assert(tcp_socket.getPort() == tcp_socket_copy.getPort());
		assert(tcp_socket.getListenSd() == tcp_socket_copy.getListenSd());
		assert(tcp_socket.getBacklog() == tcp_socket_copy.getBacklog());
		assert(tcp_socket.getSocketAddress().sin_family ==
			   tcp_socket_copy.getSocketAddress().sin_family);
		assert(
			tcp_socket.getSocketAddress().sin_port == tcp_socket_copy.getSocketAddress().sin_port);
		assert(tcp_socket.getSocketAddress().sin_addr.s_addr ==
			   tcp_socket_copy.getSocketAddress().sin_addr.s_addr);
		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		std::cout << GREEN << "copy assignment operator test: ";
		server::TcpSocket tcp_socket("127.0.0.1", "8080");
		server::TcpSocket tcp_socket_copy = tcp_socket;
		assert(tcp_socket.getIpAddress() == tcp_socket_copy.getIpAddress());
		assert(tcp_socket.getPort() == tcp_socket_copy.getPort());
		assert(tcp_socket.getListenSd() == tcp_socket_copy.getListenSd());
		assert(tcp_socket.getBacklog() == tcp_socket_copy.getBacklog());
		assert(tcp_socket.getSocketAddress().sin_family ==
			   tcp_socket_copy.getSocketAddress().sin_family);
		assert(
			tcp_socket.getSocketAddress().sin_port == tcp_socket_copy.getSocketAddress().sin_port);
		assert(tcp_socket.getSocketAddress().sin_addr.s_addr ==
			   tcp_socket_copy.getSocketAddress().sin_addr.s_addr);
		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		std::cout << GREEN << "isSocketConfigValid test: (valid address and port): ";
		server::TcpSocket tcp_socket("127.0.0.1", "8080");
		assert(tcp_socket.prepareSocketForListening() == 0);

		server::TcpSocket tcp_socket1("127.0.0.1", "8080");
		assert(tcp_socket1.prepareSocketForListening() == 0);
		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		std::cout << GREEN << "isSocketConfigValid test: (invalid address): ";
		server::TcpSocket tcp_socket2("256.256.256.256", "8080");
		assert(tcp_socket2.prepareSocketForListening() < 0);

		server::TcpSocket tcp_socket3("123.456.78.90", "8080");
		assert(tcp_socket3.prepareSocketForListening() < 0);

		server::TcpSocket tcp_socket4("192.168.1", "8080");
		assert(tcp_socket4.prepareSocketForListening() < 0);

		server::TcpSocket tcp_socket5("abc.def.ghi.jkl", "8080");
		assert(tcp_socket5.prepareSocketForListening() < 0);
		std::cout << BLUE << "passed" << RESET << std::endl;
	}

	{
		std::cout << GREEN << "createTcpSocket test: valid address and port: ";
		server::TcpSocket tcp_socket("127.0.0.1", "8080");
		assert(tcp_socket.prepareSocketForListening() == 0);

		server::TcpSocket tcp_socket1("127.0.0.1", "8080");
		assert(tcp_socket1.prepareSocketForListening() == 0);

		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		std::cout << GREEN << "enableSocketAddressReuse test: valid address and port: ";
		// アドレスとポートの再利用を確認する
		for (int i = 0; i < 10; i++) {
			{
				server::TcpSocket tcp_socket("127.0.0.1", "8080");
				assert(tcp_socket.prepareSocketForListening() == 0);
			}
		}
		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		std::cout << GREEN << "setSocketToNonBlocking test: non-blocking socket: ";
		server::TcpSocket tcp_socket("127.0.0.1", "8080");
		assert(tcp_socket.prepareSocketForListening() == 0);
		int flags = fcntl(tcp_socket.getListenSd(), F_GETFL, 0);
		assert(flags & O_NONBLOCK);
		std::cout << BLUE << "passed" << RESET << std::endl;
	}
	{
		{
			std::cout << GREEN << "configureSocketAddress test: valid address and port: ";
			server::TcpSocket tcp_socket("127.0.0.1", "8080");
			assert(tcp_socket.prepareSocketForListening() == 0);
			std::cout << BLUE << "passed" << RESET << std::endl;
		}

		{

			std::cout << GREEN << "configureSocketAddress test (invalid address ans port): ";
			server::TcpSocket tcp_socket("invalid_address", "8080");
			assert(tcp_socket.prepareSocketForListening() < 0);
			server::TcpSocket tcp_socket2("127.0.0.1", "invalid_port");
			assert(tcp_socket2.prepareSocketForListening() < 0);
			std::cout << BLUE << "passed" << RESET << std::endl;
		}
		{
			std::cout << GREEN << "bindAddressToSocket test: valid address and port: ";
			server::TcpSocket tcp_socket("127.0.0.1", "8080");
			assert(tcp_socket.prepareSocketForListening() == 0);
			std::cout << BLUE << "passed" << RESET << std::endl;
		}
		{
			std::cout << GREEN << "bindAddressToSocket test: invalid address and port: ";
			server::TcpSocket tcp_socket_invalid("invalid_address", "invalid_port");
			assert(tcp_socket_invalid.prepareSocketForListening() < 0);
			server::TcpSocket tcp_socket("127.0.0.1", "8080");
			assert(tcp_socket.prepareSocketForListening() == 0);
			assert(tcp_socket_invalid.startListening() < 0);
			server::TcpSocket tcp_socket2("127.0.0.1", "8080");
			assert(tcp_socket2.prepareSocketForListening() == 0);
			std::cout << BLUE << "passed" << RESET << std::endl;
		}
		{
			{
				std::cout << GREEN << "startListening test: valid: ";
				server::TcpSocket tcp_socket("127.0.0.1", "8080");
				assert(tcp_socket.prepareSocketForListening() == 0);
				assert(tcp_socket.startListening() == 0);
				std::cout << BLUE << "passed" << RESET << std::endl;
			}
			{
				std::cout << GREEN << "startListening test: already in use";
				server::TcpSocket tcp_socket("127.0.0.1", "8080");
				assert(tcp_socket.prepareSocketForListening() == 0);
				assert(tcp_socket.startListening() == 0);
				server::TcpSocket tcp_socket2("127.0.0.1", "8080");
				assert(tcp_socket2.prepareSocketForListening() < 0);
				std::cout << BLUE << "passed" << RESET << std::endl;
			}
		}
	}

	return 0;
}
