#ifndef IO_MULTIPLEXING_HPP
#define IO_MULTIPLEXING_HPP

#include "struct.hpp"
#include "socket.hpp"
#include <algorithm>
#include <map>
#include <vector>
#define TRUE 1
#define FALSE 0

namespace server {

class IoMultiplexing {

private:
	std::vector<socket_conf> socket_conf_;
	std::vector<server::Socket> socket_;
	std::map<int, time_t> activity_times_;
	int max_sd_;
	int max_clients_;
	struct timeval timeout_;
	fd_set master_set_;
	fd_set working_set_;
	int end_server_;
	static const time_t CONNECTION_TIMEOUT = 10;
	IoMultiplexing();

public:
	explicit IoMultiplexing(std::vector<socket_conf>& conf);
	virtual ~IoMultiplexing();
	explicit IoMultiplexing(const IoMultiplexing& other);
	IoMultiplexing& operator=(const IoMultiplexing& other);
	int initialize();
	int server_start();
	int select();
	int accept(int listen_sd);
	int request(int i);
	int disconnection(int i);
	bool isListeningSocket(int sd);
};

class IsListeningSocketPredicate {
public:
	explicit IsListeningSocketPredicate(int sd)
		: sd_(sd) {}

	bool operator()(const server::Socket& s) const { return s.getListenSd() == sd_; }

private:
	int sd_;
};

} // namespace server

#endif
