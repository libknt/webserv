#ifndef IO_MULTIPLEXING_HPP
#define IO_MULTIPLEXING_HPP

#include "../../configuration/configuration.hpp"
#include "http_response.hpp"
#include "parse_http_request.hpp"
#include "socket.hpp"
#include "struct.hpp"
#include <algorithm>
#include <map>
#include <vector>

namespace server {

class IoMultiplexing {

private:
	std::map<int, RequestProcessStatus> request_process_status_;
	std::vector<socket_conf> socket_conf_;
	IoMultiplexing();
	Configuration& configuration_;
	std::vector<server::Socket> socket_;
	std::map<int, time_t> activity_times_;
	int max_sd_;
	int max_clients_;
	struct timeval timeout_;
	fd_set master_read_fds_;
	fd_set read_fds_;
	fd_set master_write_fds_;
	fd_set write_fds_;
	bool should_stop_server_;
	static const time_t CONNECTION_TIMEOUT = 10;
	ParseHttpRequest http_request_parse_;
	std::map<int, HttpResponse> response_;

public:
	IoMultiplexing(Configuration& configuration);
	virtual ~IoMultiplexing();
	explicit IoMultiplexing(const IoMultiplexing& other);
	IoMultiplexing& operator=(const IoMultiplexing& other);
	int initialize();
	int runServer();
	int select();
	int accept(int listen_sd);
	int recv(int sd);
	int send(int sd);
	int disconnect(int sd);
	bool isListeningSocket(int sd);
	void setResponseStatus(int sd);
	bool isCgi();
	int createResponse(int sd);
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
