#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <string>
# include <map>

class location {
private:
	std::map<int, std::string> error_pages_;
	int client_max_body_size_;
	std::string root_;
public:
};

#endif