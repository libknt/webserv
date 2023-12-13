#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "cgi_handler.hpp"
#include "client_session.hpp"
#include "configuration.hpp"
#include "handle_request.hpp"
#include "http_request_parser.hpp"
#include "server_cgi_utils.hpp"
#include "tcp_socket.hpp"
#include "utils.hpp"
#include "webserv.hpp"
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <vector>

namespace server {

class ServerManager {

private:
	const Configuration& configuration_;
	std::vector<server::TcpSocket> sockets_;
	std::map<int, ClientSession> active_client_sessions_;
	std::map<int, int> cgi_socket_pairs_;
	fd_set master_read_fds_;
	fd_set master_write_fds_;
	fd_set read_fds_;
	fd_set write_fds_;
	int highest_sd_;
	bool is_running;
	struct timeval timeout_;

	ServerManager();
	int setupServerSockets();
	int setupSelectReadFds();
	int monitorSocketEvents();
	int dispatchSocketEvents(int ready_sds);
	bool isListeningSocket(int sd);
	int acceptIncomingConnection(int listen_sd);
	int setNonBlocking(int sd);
	int receiveAndParseHttpRequest(ClientSession& client_session);
	void setClientResponseStage(ClientSession& session);
	int setWriteFd(int sd);
	int setReadFd(int sd);
	int sendResponse(ClientSession& client_session);

	void registerClientSession(int sd, sockaddr_in client_address, sockaddr_in server_address);
	void closeClientSession(ClientSession& client_session);
	int unregisterClientSession(ClientSession& client_session);
	ClientSession& getClientSession(int const sd);
	int sendCgiBody(ClientSession& client_session);
	int resolveClientSocket(const int sd);
	void cleaning(ClientSession &client_sesion);

public:
	ServerManager(const Configuration& configuration);
	virtual ~ServerManager();
	explicit ServerManager(const ServerManager& other);
	ServerManager& operator=(const ServerManager& other);
	int runServer();

	Configuration const& getConfiguration() const;
	std::vector<server::TcpSocket> const& getSockets() const;
	std::map<int, ClientSession> const& getActiveClientSessions() const;
	fd_set const& getMasterReadFds() const;
	fd_set const& getMasterWriteFds() const;
	fd_set const& getReadFds() const;
	fd_set const& getWriteFds() const;
	void clearFds(int sd);
	int getHighestSd() const;
	bool getIsRunning() const;
	struct timeval const& getTimeout() const;
};

std::ostream& operator<<(std::ostream& out, const ServerManager& server_manager);

} // namespace server

#endif
