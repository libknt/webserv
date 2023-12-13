#include "logger.hpp"

Logger::Logger(LogLevel level, std::string const& filename)
	: currentLevel(level) {
	std::cout << "Logger constructor" << std::endl;
	std::cout << "filename: " << filename << std::endl;
	log_file_.open(filename.c_str(), std::ios::out | std::ios::app);
}

Logger::~Logger() {
	log_file_.close();
}

Logger& Logger::getInstance(LogLevel level) {
	static Logger instance(level);
	return instance;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
	if (level >= currentLevel) {
		log_file_ << std::left << std::setw(8) << LogLevelToString(level) << ":";
		log_file_ << file << ":" << line << "  " << message << std::endl;
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
