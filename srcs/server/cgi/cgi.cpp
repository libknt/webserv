#include "cgi.hpp"
#include <cstdlib>
extern char** environ;
namespace server {

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

int Cgi::exec_cgi() {
	int status = 0;
	std::string script = meta_.find_meta_variable("SCRIPT_NAME");
	std::string path;
	script = "." + script;
	shebang(script, path);
	if (path.empty())
		extension(script, path);
	char* argv[] = { (char*)path.c_str(), (char*)script.c_str(), NULL };
	char **exec_env = meta_.get_exec_environ();
	pid_t pid = fork();
	if (pid == 0) {
		std::cout << path;
		int err = execve(path.c_str(), argv, exec_env);
		exit(err);
	}
	waitpid(pid, &status, WIFEXITED(status));
	if (status !=0) {
		std::cerr << "exec_err(): " << strerror(errno) << std::endl;
		exit(-1); 
	} 
	return status;
}



#include "debug.hpp"

int Cgi::cgi_request() {
	if (meta_.create_meta_variables() < 0)
		return -1;
	meta_.get_meta();
	std::cout << YELLOW;
	exec_cgi();
	// for (char** current = environ; *current; ++current) {
	// 	std::cout << *current << std::endl;
	// }
	std::cout << RESET;
	return 0;
}

}