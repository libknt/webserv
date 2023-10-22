#include "configuration/configuration.hpp"

int setConfigration(int argc, char** argv, Configuration& configuration) {
	// resultができれば使いたい
	if (argc == 1) {
		parseConfiguration(configuration, "configuration/default.conf");
	} else {
		parseConfiguration(configuration, argv[1]);
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