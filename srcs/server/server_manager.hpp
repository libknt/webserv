#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "configuration.hpp"
#include "handle_request.hpp"
#include "parse_http_request.hpp"
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
	fd_set master_write_fds_;
	fd_set read_fds_;
	fd_set write_fds_;
	int highest_sd_;
	bool is_running;
	struct timeval timeout_;
	ParseHttpRequest http_request_parse_;
	std::map<int, SERVER_STATUS> server_status_;
	std::map<int, HttpResponse> response_;

	ServerManager();
	int setupServerSockets();
	int setupSelectReadFds();
	int monitorSocketEvents();
	int dispatchSocketEvents(int ready_sds);
	bool isListeningSocket(int sd);
	int acceptIncomingConnection(int listen_sd);
	int createsServerStatus(int sd);
	int receiveAndParseHttpRequest(int sd);
	int determineIfCgiRequest(int sd);
	void decomposeCgiUrl(const std::string& path,
		std::string& location,
		std::string& scriptFileName);
	std::string extractScriptFileName(std::string const& path);
	void sanitizePath(std::string& path);
	std::string extractParentDirectoryPath(std::string const& path);
	int setWriteFd(int sd);
	int sendResponse(int sd);
	int requestCleanup(int sd);
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
