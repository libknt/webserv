#include "io_multiplexing.hpp"

int main(int argc, char** argv) {
	Configuration configuration;

	switch (argc) {
		case 1:
			configuration.init("configuration/default.conf");
			break;
		case 2:
			configuration.init(argv[1]);
			break;
		default:
			return 1;
	}

	std::cout << configuration << std::endl;

	server::IoMultiplexing server(configuration);
	if (server.initialize() < 0) {
		std::cerr << "Server initialize failed" << std::endl;
		return -1;
	}
	server.runServer();
	return 0;
}