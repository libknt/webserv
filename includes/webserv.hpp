#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sstream>
#include <string>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

template <typename T>
std::string toString(const T& value);

#include "utils.tpp"

#endif