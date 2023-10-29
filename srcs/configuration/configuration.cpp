#include "configuration.hpp"

Configuration::Configuration() {}

Configuration::~Configuration() {}

Configuration::Configuration(const Configuration& other) {
	servers_ = other.servers_;
}

Configuration& Configuration::operator=(const Configuration& other) {
	if (this != &other) {
		servers_ = other.servers_;
	}
	return *this;
}

int Configuration::init(const std::string& path) {
	std::vector<std::string> tokens;
	tokens = tokenize_file_content(path);
	if (parseConfiguration(tokens) == -1) {
		return -1;
	}
	return 0;
}

std::vector<std::string> Configuration::tokenize_file_content(const std::string& path) {
	std::ifstream conf_file(path);
	std::string line;
	std::vector<std::string> tokens;

	while (getline(conf_file, line)) {
		std::vector<std::string> tmp = tokenize(line);
		tokens.insert(tokens.end(), tmp.begin(), tmp.end());
	}
	return tokens;
}

std::vector<std::string> Configuration::tokenize(std::string& line) {
	std::vector<std::string> tokens;
	std::string token;

	for (size_t i = 0; i < line.size(); ++i) {
		if (isspace(line[i]) || isSpecialCharacter(line[i])) {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			if (isSpecialCharacter(line[i])) {
				tokens.push_back(std::string(1, line[i]));
			}
		} else {
			token += line[i];
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

bool Configuration::isSpecialCharacter(const char& c) {
	return (c == '{' || c == '}' || c == ';');
}

int Configuration::parseConfiguration(std::vector<std::string>& tokens) {
	while (!tokens.empty()) {
		// TODO: rename server_tokens
		std::vector<std::string> server_tokens;
		ServerDirective server_directive;

		if (tokens.front() == "server") {
			tokens.erase(tokens.begin());
			server_tokens = ParserUtils::extractTokensFromBlock(tokens);
			server_directive.parseServerDirective(server_tokens);
			servers_.push_back(server_directive);
		} else {
			std::cerr << "Configration file error: invalid main directive." << std::endl;
			return -1;
		}
	}
	return 0;
}

std::vector<ServerDirective> Configuration::getServers() const {
	return servers_;
}

std::ostream& operator<<(std::ostream& out, const Configuration& configuration) {
	std::vector<ServerDirective> servers = configuration.getServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		out << "===== server" << i << " =====" << std::endl;
		out << servers[i] << std::endl;
	}
	return out;
}