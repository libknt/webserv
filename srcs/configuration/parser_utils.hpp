#ifndef PARSER_UTILS_HPP
#define PARSER_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

namespace ParserUtils {

std::vector<std::string> extractTokensFromBlock(std::vector<std::string>& tokens);
std::vector<std::string> extractTokensUntilSemicolon(std::vector<std::string>& tokens);
void printTokens(std::vector<std::string>& tokens);

}

#endif