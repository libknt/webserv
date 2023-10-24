#include "cgi.hpp"
#include <cstdlib>
extern char** environ;
namespace server {

// Cgi::Cgi(){};
Cgi::Cgi(HttpRequest& request)
	: request_(request) {}
Cgi::~Cgi(){};

void Cgi::getInfo() {
	this->request_.getInfo();
}

#include "debug.hpp"

int Cgi::cgi_request() {
	if (create_meta_variables() <0) 
		return -1;
	// std::cout << YELLOW;
	// for (char** current = environ; *current; ++current) {
	// 	std::cout << *current << std::endl;
	// }
	// std::cout << RESET;
	return 0;
}
// void add_auth_type(std::map<std::string, std::string> & meta) {
// 	std::string 
// }
int Cgi::create_meta_variables() {

	// add_auth_type(meta_);
	return 0;
}

}