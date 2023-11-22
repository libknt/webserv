#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
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

namespace http_error_status {
enum HTTP_ERROR_STATUS {
	BAD_REQUEST = 400,
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
	http_request_status::HTTP_REQUEST_STATUS status_;
	http_error_status::HTTP_ERROR_STATUS error_status_;
	http_method::HTTP_METHOD method_;
	std::string request_path_;
	http_version::HTTP_VERSION version_;
	std::map<std::string, std::string> header_;
	http_body_message_type::HTTP_BODY_MESSAGE_TYPE body_message_type_;
	size_t content_length_;
	chunked_status::CHUNKED_STATUS chunked_status_;
	size_t chunked_size_;
	std::string body_;
	sockaddr_in client_address_;
	sockaddr_in server_address_;
	bool isTokenDelimiter(char chr);
	bool isTokenCharacter(char chr);

	HttpRequest();

public:
	explicit HttpRequest(sockaddr_in client_address, sockaddr_in server_address);
	explicit HttpRequest(HttpRequest const& other);
	virtual ~HttpRequest();
	HttpRequest& operator=(HttpRequest const& request);
	void setStatus(http_request_status::HTTP_REQUEST_STATUS const& status);
	void setErrorStatus(http_error_status::HTTP_ERROR_STATUS const& error_status);
	int setMethod(std::string const& method);
	int setRequestPath(std::string const& request_path);
	int setVersion(std::string const& version);
	int setHeaderValue(std::string const& key, std::string const& value);
	void setBodyMassageType(
		http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& body_message_type);
	void setContentLength(size_t content_length);
	void setChunkedStatus(chunked_status::CHUNKED_STATUS const& chunked_status);
	void setChunkedSize(size_t chunked_size_);
	void appendBody(std::string const& body);
	void setClientAddress(sockaddr_in const& client_address);
	void setServerAddress(sockaddr_in const& server_address);

	http_request_status::HTTP_REQUEST_STATUS const& getStatus() const;
	http_error_status::HTTP_ERROR_STATUS const& getErrorStatus() const;
	std::string const getMethod() const;
	std::string const getVersion() const;
	std::string const& getRequestPath() const;
	std::string const getHeaderValue(std::string const& key) const;
	std::map<std::string, std::string> const& getHeader() const;
	http_body_message_type::HTTP_BODY_MESSAGE_TYPE const& getBodyMessageType();
	size_t const& getContentLength() const;
	chunked_status::CHUNKED_STATUS const& getChunkedStatus() const;
	size_t getChunkedSize() const;
	std::string const& getBody() const;
	size_t getBodySize() const;
	std::string getUriPath() const;
	std::string getUriQuery() const;
	sockaddr_in const& getClientAddress() const;
	sockaddr_in const& getServerAddress() const;
	std::string getServerIpAddress() const;
	std::string getServerPort() const;
	void cleanup();
};

std::ostream& operator<<(std::ostream& out, const HttpRequest& request);

}

#endif
