#ifndef IO_MULTIPLEXING_HPP
#define IO_MULTIPLEXING_HPP

#include "configuration.hpp"
#include "parse_http_request.hpp"
#include "socket.hpp"
#include "struct.hpp"
#include <map>
#include <vector>

namespace server {

enum SERVER_STATUS {
	REQUEST,
	RESPONSE,
	CGI,
	FINISHED,
	ERROR,
	UNDEFINED,
};

class IoMultiplexing {

private:
	IoMultiplexing();
	Configuration& configuration_;
	std::vector<server::Socket> socket_;
	int highest_socket_descriptor_;
	struct timeval timeout_;
	fd_set master_read_fds_;
	fd_set read_fds__;
	bool server_is_running_;
	ParseHttpRequest http_request_parse_;
	int dispatchSocketEvents(int ready_descriptors);
	int setupSelectReadFds();
	int setUpServerSockets();
	int monitorSocketEvents();
	int acceptIncomingConnection(int listen_sd);
	int receiveAndProcessData(int sd);
	int disconnect(int sd);
	bool isListeningSocket(int sd);

public:
	IoMultiplexing(Configuration& configuration);
	virtual ~IoMultiplexing();
	explicit IoMultiplexing(const IoMultiplexing& other);
	IoMultiplexing& operator=(const IoMultiplexing& other);
	int runServer();
};

} // namespace server

#endif
