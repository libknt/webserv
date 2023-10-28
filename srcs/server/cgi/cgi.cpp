#include "cgi.hpp"
#include <cstdlib>
extern char** environ;
namespace server {
//clsoe signal がミスってcloseできない場合がある。signalが来るとcloseされない場合がある。
// Cgi::Cgi(){};
Cgi::Cgi(HttpRequest& request)
	: meta_(CgiMetaVariables(request))
	, request_(request) {}
Cgi::~Cgi(){};

void Cgi::getInfo() {
	this->request_.getInfo();
}

std::string Cgi::getFileExtension(const std::string& filename) {
	size_t pos = filename.find_last_of('.');
	if (pos != std::string::npos) {
		return filename.substr(pos);
	}
	return "";
}

int Cgi::extension(std::string filename, std::string& path) {
	std::map<std::string, std::string> interpreterMap;

	interpreterMap.insert(std::make_pair(".py", "/usr/local/bin/python3"));
	interpreterMap.insert(std::make_pair(".pl", "/usr/bin/perl"));
	interpreterMap.insert(std::make_pair(".sh", "/bin/bash"));
	interpreterMap.insert(std::make_pair(".rb", "/usr/bin/ruby"));

	std::string extension = getFileExtension(filename);
	path.clear();
	for (std::map<std::string, std::string>::iterator it = interpreterMap.begin();
		 it != interpreterMap.end();
		 ++it) {
		if (it->first == extension) {
			path = it->second;
			break;
		}
	}
	return 0;
}

// シバン（shebang）
// #!/usr/local/bin/python3
int Cgi::shebang(std::string file, std::string& path) {
	std::ifstream infile(file.c_str());
	if (!infile) {
		std::cout << "file open error: " << file << std::endl;
		return -1;
	}
	std::string line;
	std::getline(infile, line);
	path.clear();
	if (strncmp(line.c_str(), "#!", 2) == 0) {
		line = line.substr(2);
		int access_flag = access(line.c_str(), X_OK);
		if (access_flag == 0) {
			path = line;
		}
	}
	return 0;
}

int Cgi::createExecArgv() {
	std::string script = meta_.findMetaVariable("SCRIPT_NAME");
	std::string path;
	script = "." + script;
	shebang(script, path);
	if (path.empty())
		extension(script, path);

	path_ = path;
	script_ = script;
	return 0;
}

int Cgi::execCgi() {
	int status = 0;
	int sv[2];
	createExecArgv();
	this->body_ = request_.getBody();
	char** exec_env = meta_.getExecEnviron();
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
		perror("socketpair");
		std::exit(1);
	}
	// fork < 0
	pid_t pid = fork();
	if (pid == 0) {
		close(sv[0]);
		char buffer[4096];
		// dup2 < 0
		dup2(sv[1], STDOUT_FILENO);
		dup2(sv[1], STDIN_FILENO);
		std::memset(buffer, '\0', sizeof(buffer));
		ssize_t bytes_read;
		bytes_read = read(1, buffer, sizeof(buffer));
		std::string ss(buffer);
		char* exec_argv[] = {
			(char*)path_.c_str(), (char*)script_.c_str(), (char*)ss.c_str(), NULL
		};
		int err = execve(path_.c_str(), exec_argv, exec_env);
		std::exit(err);
	}
	close(sv[1]);

	write(sv[0], body_.c_str(), body_.length());

 //TODO timeoutの設定をする
	bool b = false;
	for (;;) {
		int status;
		if (waitpid(pid, &status, WNOHANG) != 0) {
			std::cout << "Child process ended." << std::endl;
			b = true;
			break;
		}
		sleep(1);
	}
	// kill < 0
	if (!b)
		kill(pid, SIGKILL);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		std::cerr << "exec_err(): " << strerror(errno) << std::endl;
		std::exit(-1);
	}

	char buffer[4096];
	ssize_t bytes_read;
	while ((bytes_read = read(sv[0], buffer, sizeof(buffer))) > 0) {
		std::cout.write(buffer, bytes_read);
	}
	close(sv[0]);

	return status;
}

#include "debug.hpp"

int Cgi::cgiRequest() {
	if (meta_.createMetaVariables() < 0)
		return -1;
	meta_.getMeta();
	std::cout << YELLOW;
	execCgi();
	std::cout << RESET;
	return 0;
}

}