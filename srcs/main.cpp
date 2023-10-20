#include <iostream>
#include <unistd.h>
int main() {
	int pid = fork();
	int status;

	if (pid == 0) {
		char* argv[] = {
			(char*)"/Users/kyoda/sgoinfre/.brew/bin/python3",
			(char*)"/Users/kyoda/Desktop/webserv/cgi/hello.py",
			NULL
		};
		char* envp[] = {
			NULL
		};

		execve("/Users/kyoda/sgoinfre/.brew/bin/python3", argv, envp);

		perror("execve");
		exit(EXIT_FAILURE);
	}

	waitpid(pid, &status, 0);
	return 0;
}
