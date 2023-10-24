#include "io_multiplexing.hpp"
#include "socket.hpp"

int main() {
	std::vector<socket_conf> c;
	socket_conf sconf;
	const char* addr = "127.0.0.1";
	int port = 1050;

	for (int i = 0; i < 100; ++i) {
		sconf.addr = addr;
		sconf.port = port++;
		c.push_back(sconf);
	}
	port = 1050;
	for (int i = 0; i < 100; ++i) {
		sconf.addr = addr;
		sconf.port = port++;
		c.push_back(sconf);
	}

	const char* addresses[] = { "127.0.0.2", "127.0.0.3", "127.0.0.4", "127.0.0.5", "127.0.0.6" };

	for (int i = 0; i < 5; ++i) {
		sconf.addr = addresses[i];
		sconf.port = port++;
		c.push_back(sconf);
	}

	port = 8080;
	for (int i = 0; i < 5; ++i) {
		sconf.addr = addr;
		sconf.port = port++;
		c.push_back(sconf);
	}

	server::IoMultiplexing server(c);
	server.initialize();
	server.server_start();
	return 0;
}
