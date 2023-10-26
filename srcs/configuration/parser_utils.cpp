#include "parser_utils.hpp"

namespace ParserUtils {

    std::vector<std::string> extractTokensFromBlock(std::vector<std::string>& tokens) {
        std::vector<std::string> extracted_tokens;
        size_t num_of_left_brace = 0;
        size_t num_of_right_brace = 0;

        if (tokens.empty() || tokens.front() != "{") {
            std::cerr << "Parse Error: Invalid Block" << std::endl;
            return extracted_tokens;
        }
        while (!tokens.empty()) {
            std::string token = tokens.front();
            tokens.erase(tokens.begin());
            if (token == "{") {
                num_of_left_brace++;
            } else if (token == "}") {
                num_of_right_brace++;
            } else {
                extracted_tokens.push_back(token);
            }
            if (num_of_left_brace == num_of_right_brace) {
                break;
            }
        }
        if (num_of_left_brace != num_of_right_brace) {
            std::cerr << "Parse Error: Invalid Block" << std::endl;
            extracted_tokens.clear();
            return extracted_tokens;
        }
        return extracted_tokens;
    }

    // セミコロンがない場合の処理
    std::vector<std::string> extractTokensUntilSemicolon(std::vector<std::string>& tokens) {
        std::vector<std::string> extracted_tokens;
        bool has_semicolon = false;

        while (!tokens.empty()) {
            std::string token = tokens.front();
            tokens.erase(tokens.begin());
            if (token == ";") {
                has_semicolon = true;
                break;
            }
            extracted_tokens.push_back(token);
        }
        if (!has_semicolon) {
            std::cerr << "Parse Error: Invalid Simple Directive" << std::endl;
            extracted_tokens.clear();
            return extracted_tokens;
        }
        return extracted_tokens;
    }

}