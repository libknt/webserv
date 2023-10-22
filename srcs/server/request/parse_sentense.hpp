#ifndef	PARSE_SENTENSE_HPP
#define	PARSE_SENTENSE_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <aio.h>

enum TokenKind
{
	STRING,
	KEYWORD,
	SPACE,
};

struct Token
{
	TokenKind	token_kind;
	std::string	str;
};
int		parse_sentense(std::string line, std::string const &format, std::vector<std::string> &ans);
#endif
