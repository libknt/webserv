#include "parser_utils.hpp"

namespace ParserUtils {

    std::list<std::string> extractTokensFromBlock(std::list<std::string>& tokens) {
        std::list<std::string> extracted_tokens;
        size_t num_of_left_brace = 0;
        size_t num_of_right_brace = 0;

        if (tokens.empty() || tokens.front() != "{") {
            std::cout << tokens.front() << std::endl;
            std::cerr << "Parse Error: Invalid Block" << std::endl;
            return std::list<std::string>();
        }

        while (!tokens.empty()) {
            std::string token = tokens.front();
            tokens.erase(tokens.begin());
            if (token == "{") {
                num_of_left_brace++;
                if (num_of_left_brace == 1) {
                    continue;
                }
            } else if (token == "}") {
                num_of_right_brace++;
                if (num_of_left_brace == num_of_right_brace) {
                    return extracted_tokens;
                }
            }
            extracted_tokens.push_back(token);
        }

        std::cerr << "Parse Error: Invalid Block" << std::endl;
        return std::list<std::string>();
    }

    std::list<std::string> extractTokensUntilSemicolon(std::list<std::string>& tokens) {
        std::list<std::string> extracted_tokens;
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

    void printTokens(std::list<std::string>& tokens) {
        std::cout << "========== printTokens() ==========" << std::endl;
        for (std::list<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
            std::cout << "[" << *it << "] " << " ";
        }
        std::cout << std::endl;
    }

}