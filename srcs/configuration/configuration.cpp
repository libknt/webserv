#include "configuration.hpp"

Configuration::Configuration() {}

Configuration::~Configuration() {}

Configuration::Configuration(const Configuration& other)
	: servers_(other.servers_) {}

Configuration& Configuration::operator=(const Configuration& other) {
	if (this != &other) {
		servers_ = other.servers_;
	}
	return *this;
}

int Configuration::init(const std::string& path) {
	if (path.size() < 5 || path.find(".conf", path.size() - 5) == std::string::npos) {
		std::cerr << "File Error: The path contains an incorrect file extension" << std::endl;
	}

	std::ifstream conf_file(path.c_str());
	if (conf_file.fail()) {
		std::cerr << "File Error: Open conf file failed" << std::endl;
		return -1;
	}

	std::vector<std::string> tokens;
	tokens = tokenize_file_content(conf_file);
	if (parseConfiguration(tokens) == -1) {
		return -1;
	}
	return 0;
}

std::vector<std::string> Configuration::tokenize_file_content(std::ifstream& conf_file) {
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
		std::vector<std::string> server_tokens;
		ServerDirective server_directive;

		if (tokens.front() == "server") {
			tokens.erase(tokens.begin());

			server_tokens = ParserUtils::extractTokensFromBlock(tokens);
			if (server_directive.parseServerDirective(server_tokens) == -1) {
				return -1;
			}

			for (size_t i = 0; i < servers_.size(); ++i) {
				if (server_directive.getIpAddress() == servers_[i].getIpAddress() &&
					server_directive.getPort() == servers_[i].getPort()) {
					std::cerr << "Parse Error: Duplicate port number" << std::endl;
					return -1;
				}
			}
			servers_.push_back(server_directive);
		} else {
			std::cerr << "Parse Error: Invalid main directive" << std::endl;
			return -1;
		}
	}
	return 0;
}

const std::vector<ServerDirective>& Configuration::getServers() const {
	return servers_;
}

ServerDirective const& Configuration::getServerDirective(std::string const& ip_address,
	std::string const& port) const {
	size_t i = 0;
	while (i < servers_.size()) {
		if (servers_[i].getIpAddress() == ip_address && servers_[i].getPort() == port) {
			break;
		}
		++i;
	}
	return servers_[i];
}

std::ostream& operator<<(std::ostream& out, const Configuration& configuration) {
	std::vector<ServerDirective> servers = configuration.getServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		out << "===== server" << i << " =====" << std::endl;
		out << servers[i] << std::endl;
	}
	return out;
}