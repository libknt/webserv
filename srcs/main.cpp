#include "configuration/configuration.hpp"

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
	// start();
	return 0;
}