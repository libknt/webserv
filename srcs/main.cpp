#include "server.hpp"

int conf_setting() {
	return false;
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	Server s;

	if (conf_setting()) {
		return 1;
	}
	s.start();
	return 0;
}