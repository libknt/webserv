#include "parser_utils.hpp"

namespace ParserUtils {

    std::vector<std::string> extractTokensFromBlock(std::vector<std::string>& tokens) {
        std::vector<std::string> extracted_tokens;
        size_t i;
        int num_of_left_brace = 1;
        int num_of_right_brace = 0;

        // TODO: エラー処理
        if (tokens.front() != "{") {
            std::cerr << "non first brace error" << std::endl;
            return extracted_tokens;
        }
        for (i = 2; i < tokens.size() && num_of_left_brace != num_of_right_brace; ++i) {
            if (tokens[i] == "{") {
                num_of_left_brace++;
            } else if (tokens[i] == "}") {
                num_of_right_brace++;
            } else {
                extracted_tokens.push_back(tokens[i]);
            }
        }
        if (num_of_left_brace != num_of_right_brace) {
            std::cerr << "non first brace error" << std::endl;
        }
        tokens.erase(tokens.begin(), tokens.begin() + i);
        return extracted_tokens;
    }

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