#ifndef PARSE_SENTENSE_HPP
#define PARSE_SENTENSE_HPP

#include <aio.h>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include "rules.hpp"

namespace server {

enum TokenKind {
	STRING,
	KEYWORD,
	SPACE,
};

struct Token {
	TokenKind token_kind;
	std::string str;
};
int parseSentense(std::string line,
	std::string const& format,
	std::vector<std::string>& parsed_line);
}
#endif
