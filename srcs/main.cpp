#include "server_manager.hpp"

int main(int argc, char** argv) {
	if (BUFFER_SIZE <= 1) {
		std::cerr << "buffer size error: " << BUFFER_SIZE << std::endl;
		return (1);
	}
	Configuration configuration;

	switch (argc) {
		case 1:
			if (configuration.init("configuration/default.conf") == -1) {
				return 1;
			}
			break;
		case 2:
			if (configuration.init(argv[1]) == -1) {
				return 1;
			}
			break;
		default:
			return 1;
	}
	std::cout << configuration << std::endl;

	server::ServerManager server(configuration);
	if (server.runServer() != -1) {
		std::cerr << "runServer() failed" << std::endl;
		return -1;
	}
	return 0;
}
