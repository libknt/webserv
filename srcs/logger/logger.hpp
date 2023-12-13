#ifndef LOGGER_HPP
#define LOGGER_HPP

#define ROOT "../../"
#define LOGFILE "log.log"
#define COLOR_SWITCH "ON"
#define LOG(level, message) Logger::getInstance().log(level, message, __FILE__, __LINE__)

#include <iomanip>
#include <iostream>
#include <string>

enum LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
private:
	LogLevel currentLevel;
	static Logger* instance;

	Logger(LogLevel level);

	Logger(const Logger&);
	Logger& operator=(const Logger&);

public:
	static Logger& getInstance(LogLevel level = INFO);

	void log(LogLevel level, const std::string& message, const std::string& file, int line);
	std::string LogLevelToString(LogLevel level);

	void setLevel(LogLevel level);
};

#endif