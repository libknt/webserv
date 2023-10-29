#ifndef LOCATION_DIRECTIVE_HPP
# define LOCATION_DIRECTIVE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "parser_utils.hpp"

class LocationDirective {
private:
	std::vector<std::string> error_page_;
	std::vector<std::string> allow_methods_;
	std::string client_max_body_size_;
	std::string root_;
	std::string index_;
	std::string autoindex_;
	std::string chunked_transfer_encoding_;

	int parseErrorPageDirective(std::vector<std::string>& tokens);
	int parseClientMaxBodySizeDirective(std::vector<std::string>& tokens);
	int parseRootDirective(std::vector<std::string>& tokens);
	int parseIndexDirective(std::vector<std::string>& tokens);
	int parseAutoindexDirective(std::vector<std::string>& tokens);
	int parseAllowMethodsDirective(std::vector<std::string>& tokens);
	int parseChunkedTransferEncodingDirective(std::vector<std::string>& tokens);
public:
	LocationDirective();
	~LocationDirective();
	LocationDirective(const LocationDirective& other);
	LocationDirective& operator=(const LocationDirective& other);

	int parseLocationDirective(std::string& location_path, std::vector<std::string>& tokens);
};

#endif