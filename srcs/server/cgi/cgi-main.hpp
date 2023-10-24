#ifndef CGI_MAIN_HPP
#define CGI_MAIN_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

int url_query_parser(const char* url, std::string& script, std::string& query);
int shebang(std::string file, std::string& path);
int extension(std::string filename, std::string& path);
#endif