#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "utils.hpp"
#include "webserv.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

namespace server {

namespace http_response_status {
enum HTTP_RESPONSE_STATUS {
	RESPONSE_SENDING,
	FINISHED,
	ERROR,
	UNDEFINED,
};
} // namespace http_response_status

class HttpResponse {
private:
	http_status_code::STATUS_CODE status_code_;
	std::map<std::string, std::string> header_;
	std::string body_;
	http_response_status::HTTP_RESPONSE_STATUS status_;
	std::stringstream stream_;

public:
	HttpResponse();
	~HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	void concatenateComponents();
	std::string statusCodeToStatusText(const http_status_code::STATUS_CODE code);
	void setStatus(const http_response_status::HTTP_RESPONSE_STATUS& status);
	void setStatusCode(const http_status_code::STATUS_CODE& status_code);
	void setHeaderValue(const std::string& key, const std::string& value);
	void setBody(const std::string& body);
	void appendBody(const std::string& body);
	http_status_code::STATUS_CODE getStatusCode() const;
	const std::string getFileContentType(std::string const& file_name) const;
	const std::string getHeaderValue(const std::string& key) const;
	const std::map<std::string, std::string>& getHeader() const;
	const std::string& getBody() const;
	std::string::size_type getBodyLength() const;
	http_response_status::HTTP_RESPONSE_STATUS const& getStatus() const;
	void getStreamBuffer(char* buffer, size_t buffer_size);
};

std::ostream& operator<<(std::ostream& out, const HttpResponse& response);
}

#endif