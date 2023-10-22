#include "location_directive.hpp"

LocationDirective::LocationDirective() {}

LocationDirective::~LocationDirective() {}

LocationDirective::LocationDirective(const LocationDirective& other) {
	error_pages_ = other.error_pages_;
	client_max_body_size_ = other.client_max_body_size_;
	root_ = other.root_;
	index_ = other.index_;
	autoindex_ = other.autoindex_;
	allow_methods_ = other.allow_methods_;
	chunked_transfer_encoding_ = other.chunked_transfer_encoding_;
}

LocationDirective& LocationDirective::operator=(const LocationDirective& other) {
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
