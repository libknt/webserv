#include "server.hpp"

int conf_setting() {
	return false;
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	if (conf_setting()) {
		return 1;
	}
	start();
	return 0;
}