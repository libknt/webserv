#include "http_response.hpp"

namespace server {

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

int HttpResponse::initialize() {
	return 0;
}

RequestProcessStatus HttpResponse::finish() {
	return SEND;
}

} // namespace server