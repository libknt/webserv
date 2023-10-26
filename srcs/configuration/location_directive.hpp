#ifndef LOCATION_DIRECTIVE_HPP
# define LOCATION_DIRECTIVE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "parser_utils.hpp"

class LocationDirective {
private:
	std::map<int, std::string> error_pages_;
	int client_max_body_size_;
	std::string root_;
	std::string index_;
	bool autoindex_;
	std::vector<std::string> allow_methods_;
	bool chunked_transfer_encoding_;
public:
	LocationDirective();
	~LocationDirective();
	LocationDirective(const LocationDirective& other);
	LocationDirective& operator=(const LocationDirective& other);

	int parseLocationDirective(std::string& location_path, std::vector<std::string>& tokens);
};

#endif