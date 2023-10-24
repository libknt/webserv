#ifndef SERVER_HPP
#define SERVER_HPP

#include "cgi.hpp"
#include "io_multiplexing.hpp"
#include "struct.hpp"
#include <iostream>
#include <vector>
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

class Server {
public:
	explicit Server();
	~Server();

	int start();
};

#endif