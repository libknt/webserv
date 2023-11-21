#ifndef LOCATION_DIRECTIVE_HPP
#define LOCATION_DIRECTIVE_HPP

#include "parser_utils.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

class LocationDirective {
private:
	const std::string location_path_;
	std::map<std::string, std::string> error_pages_;
	std::vector<std::string> allow_methods_;
	std::string client_max_body_size_;
	std::string root_;
	std::string index_;
	std::string autoindex_;
	std::map<std::string, std::string> return_;
	std::string chunked_transfer_encoding_;
	std::string cgi_;
	std::vector<std::string> cgi_extensions_;

	int parseErrorPageDirective(std::vector<std::string>& tokens);
	int parseClientMaxBodySizeDirective(std::vector<std::string>& tokens);
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
	std::map<std::string, std::string> getErrorPages() const;
	std::vector<std::string> getAllowMethods() const;
	std::string getClientMaxBodySize() const;
	std::string getRoot() const;
	std::string getIndex() const;
	std::string getAutoindex() const;
	std::map<std::string, std::string> const& getReturn() const;
	std::string getChunkedTransferEncoding() const;
	std::string getCgi() const;
	const std::vector<std::string>& getCgiExtensions() const;
	bool isValidCgiExtensions(const std::string& extension) const;
};

std::ostream& operator<<(std::ostream& out, const LocationDirective& location_directive);

#endif
