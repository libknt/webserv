#include "configuration/configuration.hpp"

int main(int argc, char** argv) {
	Configuration configuration;

	switch (argc) {
	case 1:
		configuration.setConfiguration("configuration/default.conf");
		break;
	case 2:
		configuration.setConfiguration(argv[1]);
		break;
	default:
		return 1;
	}
	return 0;
}