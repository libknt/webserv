#ifndef CGI_HPP
#define CGI_HPP

namespace server {

class Cgi {
private:
	int socket_vector_[2];
	int pid_;
	int status_;

public:
	Cgi();
	Cgi(const Cgi& other);
	Cgi& operator=(const Cgi& other);
	~Cgi();
};

} // namespace server

#endif // CGI_HPP