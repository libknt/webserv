#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include "webserv.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>

namespace server {
namespace http_request_status {
enum HTTP_REQUEST_STATUS {
	METHOD,
	HEADER,
	BODY,
	FINISHED,
	ERROR,
	UNDEFINED,
};
}

namespace http_method {
enum HTTP_METHOD {
	GET,
	POST,
	DELETE,
	UNDEFINED,
};
}

namespace chunked_status {
enum CHUNKED_STATUS {
	CHUNKED_SIZE,
	CHUNKED_MESSAGE,
	UNDEFINED,
};
}

namespace http_version {
enum HTTP_VERSION {
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0,
	UNDEFINED,
};
}

namespace http_body_message_type {
enum HTTP_BODY_MESSAGE_TYPE {
	CONTENT_LENGTH,
	CHUNK_ENCODING,
	NONE,
	UNDEFINED,
};
}

class HttpRequest {
private:
	std::string stream_line_;
	http_request_status::HTTP_REQUEST_STATUS status_;
	http_status_code::STATUS_CODE status_code_;
	http_method::HTTP_METHOD method_;
	std::string uri_;
	http_version::HTTP_VERSION version_;
	std::map<std::string, std::string> header_;
	http_body_message_type::HTTP_BODY_MESSAGE_TYPE body_message_type_;
	size_t content_length_;
	chunked_status::CHUNKED_STATUS chunked_status_;
	size_t chunked_size_;
	std::string body_;
	// sockaddr_in client_address_;
	// sockaddr_in server_address_;
	bool isTokenDelimiter(char chr);
	bool isTokenCharacter(char chr);

public:
	HttpRequest();
	HttpRequest(HttpRequest const& other);
	virtual ~HttpRequest();
	HttpRequest& operator=(HttpRequest const& request);
	void appendStreamLine(std::string const& stream_line);
	void setStreamLine(std::string const& stream_line);
	void eraseStreamLine(std::string::size_type position, std::string::size_type n);
	void setStatus(http_request_status::HTTP_REQUEST_STATUS const& status);
	void setHttpStatusCode(http_status_code::STATUS_CODE const& status_code);
	int setMethod(std::string const& method);
	int setUri(std::string const& uri);
	int setVersion(std::string const& version);
	int setHeaderValue(std::string const& key, std::string const& value);
	void setBodyMassageType(
		http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& body_message_type);
	void setContentLength(size_t content_length);
	void setChunkedStatus(chunked_status::CHUNKED_STATUS const& chunked_status);
	void setChunkedSize(size_t chunked_size_);
	void appendBody(std::string const& body);

	std::string const& getStreamLine() const;
	http_request_status::HTTP_REQUEST_STATUS const& getStatus() const;
	http_status_code::STATUS_CODE const& getHttpStatusCode() const;
	std::string const getMethod() const;
	http_method::HTTP_METHOD getHttpMethod() const;
	std::string const getVersion() const;
	std::string const& getUri() const;
	std::string getUriScheme() const;
	std::string getUriAuthority() const;
	std::string getUriPath() const;
	std::string getUriQuery() const;
	std::string const getHeaderValue(std::string const& key) const;
	std::map<std::string, std::string> const& getHeader() const;
	http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& getBodyMessageType() const;
	size_t const& getContentLength() const;
	chunked_status::CHUNKED_STATUS const& getChunkedStatus() const;
	size_t getChunkedSize() const;
	std::string const& getBody() const;
	size_t getBodySize() const;
};

std::ostream& operator<<(std::ostream& out, const HttpRequest& request);

}

#endif
