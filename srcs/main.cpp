// script urlだけ送られてきて実行する
#include "server.hpp"
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

int cgi(std::string url) {
	int status;
	std::string script("");
	std::string query("");
	url_query_parser(url.c_str(), script, query);
	extern char** environ;

	std::string path;
	script = "../" + script;
	shebang(script, path);
	if (path.empty())
		extension(script, path);
	char* argv[] = { (char*)path.c_str(), (char*)script.c_str(), NULL };
	pid_t pid = fork();
	if (pid == 0) {
		std::cout << path;
		int err = execve(path.c_str(), argv, environ);
		exit(err);
	}
	waitpid(pid, &status, WIFEXITED(status));
	return 0;
}

int main() {
	if (cgi(std::string("https://localhost.com/cgi-bin/hello.py/key=value")) < 0)
		std::cerr << "cgi error" << std::endl;
	if (cgi(std::string("https://localhost.com/cgi-bin/hello.cgi?key=value")) < 0)
		std::cerr << "cgi error" << std::endl;
	return 0;
}