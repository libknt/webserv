#ifndef LOCATION_DIRECTIVE_HPP
# define LOCATION_DIRECTIVE_HPP

# include <iostream>
# include <string>
# include <map>
# include <list>
# include "parser_utils.hpp"

class LocationDirective {
private:
	std::list<std::string> error_page_;
	std::list<std::string> allow_methods_;
	std::string client_max_body_size_;
	std::string root_;
	std::string index_;
	std::string autoindex_;
	std::string chunked_transfer_encoding_;

	int parseErrorPageDirective(std::list<std::string>& tokens);
	int parseClientMaxBodySizeDirective(std::list<std::string>& tokens);
	int parseRootDirective(std::list<std::string>& tokens);
	int parseIndexDirective(std::list<std::string>& tokens);
	int parseAutoindexDirective(std::list<std::string>& tokens);
	int parseAllowMethodsDirective(std::list<std::string>& tokens);
	int parseChunkedTransferEncodingDirective(std::list<std::string>& tokens);
public:
	LocationDirective();
	~LocationDirective();
	LocationDirective(const LocationDirective& other);
	LocationDirective& operator=(const LocationDirective& other);

	int parseLocationDirective(std::string& location_path, std::list<std::string>& tokens);
};

#endif