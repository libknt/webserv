#ifndef PARSER_UTILS_HPP
# define PARSER_UTILS_HPP

# include <iostream>
# include <string>
# include <list>

namespace ParserUtils {

    std::list<std::string> extractTokensFromBlock(std::list<std::string>& tokens);
    std::list<std::string> extractTokensUntilSemicolon(std::list<std::string>& tokens);
    void printTokens(std::list<std::string>& tokens);

}

#endif