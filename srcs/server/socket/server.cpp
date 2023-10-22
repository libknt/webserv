#include "server.hpp"

int start() {
	std::vector<socket_conf> c;
	socket_conf sconf;
	sconf.addr = "127.0.0.1";
	sconf.port = 8080;
	c.push_back(sconf);
	sconf.addr = "127.0.0.1";
	sconf.port = 8080;
	c.push_back(sconf);
	sconf.addr = "127.0.0.1";
	sconf.port = 8081;
	c.push_back(sconf);
	server::IoMultiplexing server(c);
	server.initialize();
	server.server_start();
	return 0;
}