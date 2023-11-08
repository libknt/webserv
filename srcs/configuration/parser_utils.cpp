#include "parser_utils.hpp"

namespace ParserUtils {

std::vector<std::string> extractTokensFromBlock(std::vector<std::string>& tokens) {
	std::vector<std::string> extracted_tokens;
	size_t num_of_left_brace = 0;
	size_t num_of_right_brace = 0;

	if (tokens.empty() || tokens.front() != "{") {
		std::cerr << "Parse Error: Invalid Block" << std::endl;
		return std::vector<std::string>();
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
	return std::vector<std::string>();
}

std::vector<std::string> extractTokensUntilSemicolon(std::vector<std::string>& tokens) {
	std::vector<std::string> extracted_tokens;
	bool has_semicolon = false;

	tokens.erase(tokens.begin());
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

void printTokens(std::vector<std::string>& tokens) {
	std::cout << "========== printTokens() ==========" << std::endl;
	for (size_t i = 0; i < tokens.size(); ++i) {
		std::cout << "[" << tokens[i] << "]" << std::endl;
	}
	std::cout << std::endl;
}

}