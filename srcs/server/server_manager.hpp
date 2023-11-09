#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

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

class ServerManager {

private:
	ServerManager();
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
	int receiveAndParseHttpRequest(int sd);
	int disconnect(int sd);
	bool isListeningSocket(int sd);

public:
	ServerManager(Configuration& configuration);
	virtual ~ServerManager();
	explicit ServerManager(const ServerManager& other);
	ServerManager& operator=(const ServerManager& other);
	int runServer();
};

} // namespace server

#endif
