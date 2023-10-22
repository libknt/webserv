#include "configuration/configuration.hpp"

int main(int argc, char** argv) {
	ServerDirective configuration;

	switch (argc) {
	case 1:
		configuration.setConfiguration("configuration/default.conf");
	case 2:
		configuration.setConfiguration(argv[1]);
	default:
		return 1;
	}
	return 0;
}