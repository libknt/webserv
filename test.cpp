#include <iostream>
#include <sstream>
#include <string>

int main() {
	char arr[4];
	std::stringstream stream_;
	stream_ << "ABCDEFGHIJKLMNOPQRSTUV";
	while (!stream_.eof()) {
		stream_.read(arr, 4);
		std::cout << "buffer: " << arr << std::endl;
	}
}
