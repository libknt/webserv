#ifndef LOCATION_DIRECTIVE_HPP
#define LOCATION_DIRECTIVE_HPP

#include "http_response.hpp"
#include "parser_utils.hpp"
#include "webserv.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

class LocationDirective {
private:
	const std::string location_path_;
	std::string default_error_page_;
	std::map<std::string, std::string> error_pages_;
	std::set<std::string> allow_methods_;
	std::string root_;
	std::string index_;
	bool autoindex_;
	std::string return_;
	bool chunked_transfer_encoding_;
	bool cgi_;
	std::set<std::string> cgi_extensions_;

	int parseDefaultErrorPageDirective(std::vector<std::string>& tokens);
	int parseErrorPageDirective(std::vector<std::string>& tokens);
	int parseRootDirective(std::vector<std::string>& tokens);
	int parseIndexDirective(std::vector<std::string>& tokens);
	int parseAutoindexDirective(std::vector<std::string>& tokens);
	int parseAllowMethodsDirective(std::vector<std::string>& tokens);
	int parseReturnDirective(std::vector<std::string>& tokens);
	int parseChunkedTransferEncodingDirective(std::vector<std::string>& tokens);
	int parseCgiDirective(std::vector<std::string>& tokens);
	int parseCgiExtensionsDirective(std::vector<std::string>& tokens);
	bool isStatusCode(const std::string& status_code);

public:
	LocationDirective();
	LocationDirective(const std::string& location_path);
	~LocationDirective();
	LocationDirective(const LocationDirective& other);
	LocationDirective& operator=(const LocationDirective& other);

	int parseLocationDirective(std::vector<std::string>& tokens);
	std::string getLocationPath() const;
	std::string getDefaultErrorPage() const;
	std::map<std::string, std::string> getErrorPages() const;
	std::string const findErrorPagePath(http_status_code::STATUS_CODE status_code) const;
	std::set<std::string> getAllowMethods() const;
	std::string getRoot() const;
	std::string getIndex() const;
	bool getAutoindex() const;
	std::string const& getReturn() const;
	bool getChunkedTransferEncoding() const;
	bool getCgi() const;
	bool isCgiExtension(const std::string& extension) const;
	const std::set<std::string>& getCgiExtensions() const;
	bool isAllowMethod(const std::string& method) const;
};

std::ostream& operator<<(std::ostream& out, const LocationDirective& location_directive);

#endif
