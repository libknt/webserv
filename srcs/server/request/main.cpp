#include "parse_sentense.hpp"
#include "http_request.hpp"

int main()
{
	std::cout << "------ Parse Sentense Test ------" << std::endl;
	{
		std::vector<std::string> vec;

		parse_sentense("\n", "\n", vec);
		for (size_t i = 0; i < vec.size(); i++)
			std::cout << vec[i] << std::endl;
	}
	std::cout << "---------------------------------" << std::endl;
	std::cout << "------ Http Request Parse Test------" << std::endl;
	{
		std::string line;
		HttpRequest http_request;
		while (std::getline(std::cin, line))
		{
			http_request.parseHttpRequest(line);
		}
	}
	std::cout << "---------------------------------" << std::endl;
}
