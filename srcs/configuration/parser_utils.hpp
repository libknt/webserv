#ifndef PARSER_UTILS_HPP
# define PARSER_UTILS_HPP

# include <iostream>
# include <string>
# include <vector>

namespace ParserUtils {

    std::vector<std::string> extractTokensUntilSemicolon(std::vector<std::string>& tokens) {
        std::vector<std::string> extracted_tokens;

        while (!tokens.empty()) {
            std::string token = tokens.front();
            tokens.erase(tokens.begin());
            if (token == ";") {
                break;
            }
            extracted_tokens.push_back(token);
        }
        return extracted_tokens;
    }

}

#endif