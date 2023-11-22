#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

namespace server {

class ClientSession {
private:
	/* data */

public:
	ClientSession();
	~ClientSession();
	ClientSession(const ClientSession& other);
	ClientSession& operator=(const ClientSession& other);
};

}

#endif