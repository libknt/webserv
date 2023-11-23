#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "client_session.hpp"
#include "configuration.hpp"
#include "define.hpp"
#include "http_request_parser.hpp"
#include "tcp_socket.hpp"
#include <map>
#include <vector>

namespace server {

class ServerManager {

private:
	const Configuration& configuration_;
	std::vector<server::TcpSocket> sockets_;
	std::map<int, ClientSession> active_client_sessions_;
	fd_set master_read_fds_;
	fd_set master_write_fds_;
	fd_set read_fds_;
	fd_set write_fds_;
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
	int setNonBlocking(int sd);
	int receiveAndParseHttpRequest(ClientSession& client_session);
	void determineResponseType(ClientSession& client_session);
	int setWriteFd(int sd);
	int sendResponse(ClientSession& client_session);

	void registerClientSession(int sd, sockaddr_in client_address, sockaddr_in server_address);
	void closeClientSession(ClientSession& client_session);
	int unregisterClientSession(ClientSession& client_session);
	ClientSession& getClientSession(int const sd);

public:
	ServerManager(const Configuration& configuration);
	virtual ~ServerManager();
	explicit ServerManager(const ServerManager& other);
	ServerManager& operator=(const ServerManager& other);
	int runServer();
};

} // namespace server

#endif
