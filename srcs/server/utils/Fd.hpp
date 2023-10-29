
#ifndef FD_HPP
#define FD_HPP

namespace server {

enum FD_STATUS {
	REQUEST,
	RESPONSE,
	CGI,
	FINISHED,
	ERROR,
	UNDEFINED,
};

class Fd {
private:
	int fd_;
	FD_STATUS status_;

public:
	Fd();
	explicit Fd(const int fd);
	Fd(const Fd& other);
	~Fd();
	Fd& operator=(const Fd& other);
	bool operator>(const Fd& other) const;
	bool operator<(const Fd& other) const;
	bool operator>=(const Fd& other) const;
	bool operator<=(const Fd& other) const;
	bool operator==(const Fd& other) const;
	bool operator!=(const Fd& other) const;
};
}
#endif
