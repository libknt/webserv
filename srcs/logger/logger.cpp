#include "logger.hpp"

Logger::Logger(LogLevel level)
	: currentLevel(level) {}

Logger& Logger::getInstance(LogLevel level) {
	static Logger instance(level);
	return instance;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
	if (level >= currentLevel) {
		std::cout << std::left << std::setw(8) << LogLevelToString(level) << ":";
		std::cout << file << ":" << line << "  " << message << std::endl;
	}
}

std::string Logger::LogLevelToString(LogLevel level) {
	switch (level) {
		case DEBUG:
			return "DEBUG";
		case INFO:
			return "INFO";
		case WARNING:
			return "WARNING";
		case ERROR:
			return "ERROR";
		default:
			return "UNKNOWN";
	}
}

void Logger::setLevel(LogLevel level) {
	currentLevel = level;
}
