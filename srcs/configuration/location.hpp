#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

class Location {
private:
	std::map<int, std::string> error_pages_;
	int client_max_body_size_;
	std::string root_;
	std::string index_;
	bool autoindex_;
	std::vector<std::string> allow_methods_;
	bool chunked_transfer_encoding_;
public:
	Location();
	~Location();
	Location(const Location& other);
	Location& operator=(const Location& other);
};

#endif