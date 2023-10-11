#ifndef IO_MULTIPLEXING_HPP
#define IO_MULTIPLEXING_HPP

#include "../struct.hpp"
#include "socket.hpp"
#include <map>
#include <vector>

namespace server {

class IoMultiplexing {

private:
	std::vector<socket_conf> socket_conf_;
	std::vector<server::Socket> socket_;
	int max_sd_;
	int max_clients_;
	struct timeval timeout_;
	fd_set master_set_;
	fd_set working_set_;
	IoMultiplexing();

public:
	IoMultiplexing(std::vector<socket_conf>& conf);
	virtual ~IoMultiplexing();
	IoMultiplexing(const IoMultiplexing& other);
	IoMultiplexing& operator=(const IoMultiplexing& other);
	int initialize();
	int server_start();
};

} // namespace server

#endif
