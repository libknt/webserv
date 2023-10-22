#include "location.hpp"

Location::Location() {}

Location::~Location() {}

Location::Location(const Location& other) {
	error_pages_ = other.error_pages_;
	client_max_body_size_ = other.client_max_body_size_;
	root_ = other.root_;
	index_ = other.index_;
	autoindex_ = other.autoindex_;
	allow_methods_ = other.allow_methods_;
	chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
}

Location& Location::operator=(const Location& other) {
	if (this != &other) {
		error_pages_ = other.error_pages_;
		client_max_body_size_ = other.client_max_body_size_;
		root_ = other.root_;
		index_ = other.index_;
		autoindex_ = other.autoindex_;
		allow_methods_ = other.allow_methods_;
		chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
	}
	return *this;
}
