#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "configuration.hpp"
#include "http_request_parser.hpp"
#include "tcp_socket.hpp"
#include "types.hpp"
#include <map>
#include <vector>

namespace server {

class ServerManager {

private:
	const Configuration& configuration_;
	std::vector<server::TcpSocket> sockets_;
	fd_set master_read_fds_;
	fd_set read_fds__;
	int highest_sd_;
	bool is_running;
	struct timeval timeout_;
	HttpRequestParser http_request_parser_;

	ServerManager();
	int setupServerSockets();
	int setupSelectReadFds();
	int monitorSocketEvents();
	int dispatchSocketEvents(int ready_sds);
	bool isListeningSocket(int sd);
	int acceptIncomingConnection(int listen_sd);
	int receiveAndParseHttpRequest(int sd);
	int disconnect(int sd);

public:
	ServerManager(const Configuration& configuration);
	virtual ~ServerManager();
	explicit ServerManager(const ServerManager& other);
	ServerManager& operator=(const ServerManager& other);
	int runServer();
};

} // namespace server

#endif
