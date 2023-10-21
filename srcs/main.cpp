#include "configuration.hpp"

int setConfigration(int argc, char** argv, Configuration configuration) {

	if (argc == 1) {
		configuration = parseConfiguration("configuration/default.conf");
	} else {
		configuration = parseConfiguration(argv[1]);
	}
	return 0;
}

int main(int argc, char** argv) {
	Configuration configuration;

	if (argc > 2) {
		return 1;
	}

	if (setConfigration(argc, argv, configuration)) {
		return 1;
	}
	return 0;
}