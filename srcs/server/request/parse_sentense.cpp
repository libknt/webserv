#include "parse_sentense.hpp"
static void formatTokenize(std::string const& format, std::queue<Token>& token_queue);

int parseSentense(std::string line, std::string const& format, std::vector<std::string>& ans) {
	std::queue<Token> token_queue;
	size_t index = 0;

	formatTokenize(format, token_queue);
	while (!token_queue.empty()) {
		Token token = token_queue.front();

		token_queue.pop();
		if (token.token_kind == STRING) {
			size_t past_index = index;
			if (0 < token_queue.size() && token_queue.front().token_kind == KEYWORD) {
				std::string keyword = token_queue.front().str;
				while (index < line.size() && !std::isspace(line[index])) {
					if (index + keyword.size() < line.size() &&
						line.substr(index, keyword.size()) == keyword)
						break;
					index++;
				}
			} else {
				while (index < line.size() && !std::isspace(line[index])) {
					index++;
				}
			}
			ans.push_back(line.substr(past_index, index - past_index));
		}

		else if (token.token_kind == KEYWORD) {
			std::string keyword = token.str;
			if (line.size() <= index + keyword.size() ||
				line.substr(index, keyword.size()) != keyword) {
				std::cout << "Keyword Error" << std::endl;
				ans.clear();
				return (-1);
			}
			index += keyword.size();
		}

		else if (token.token_kind == SPACE) {
			if (!std::isspace(line[index])) {
				std::cout << "Space Error" << std::endl;
				ans.clear();
				return (-1);
			}
			while (std::isspace(line[index]))
				index++;
		}
	}
	return (0);
}

static void formatTokenize(std::string const& format, std::queue<Token>& token_queue) {
	for (size_t i = 0; i < format.size(); i++) {
		if (std::isspace(format[i])) {
			Token token;
			token.token_kind = SPACE;
			token_queue.push(token);
		} else if (i + 1 < format.size() && format.substr(i, 2) == "%s") {
			Token token;
			token.token_kind = STRING;
			token_queue.push(token);
			i++;
		} else {
			if (0 < token_queue.size() && token_queue.back().token_kind == KEYWORD) {
				token_queue.back().str += format[i];
			} else {
				Token token;
				token.token_kind = KEYWORD;
				token.str = format[i];
				token_queue.push(token);
			}
		}
	}
}
