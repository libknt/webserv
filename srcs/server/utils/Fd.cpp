#include "fd.hpp"

namespace server {

Fd::Fd()
	: fd_(-1)
	, status_(UNDEFINED) {}

Fd::Fd(const int fd)
	: fd_(fd)
	, status_(UNDEFINED) {}

Fd::Fd(const Fd& other)
	: fd_(other.fd_)
	, status_(other.status_) {}

Fd::~Fd() {
}

Fd& Fd::operator=(const Fd& other) {
	if (this != &other) {
		fd_ = other.fd_;
		status_ = other.status_;
	}
	return *this;
}

bool Fd::operator>(const Fd& other) const {
	return fd_ > other.fd_;
}

bool Fd::operator<(const Fd& other) const {
	return fd_ < other.fd_;
}

bool Fd::operator>=(const Fd& other) const {
	return fd_ >= other.fd_;
}

bool Fd::operator<=(const Fd& other) const {
	return fd_ <= other.fd_;
}

bool Fd::operator==(const Fd& other) const {
	return fd_ == other.fd_;
}

bool Fd::operator!=(const Fd& other) const {
	return !(*this == other);
}

}
