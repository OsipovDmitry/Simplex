#include <iostream>

#include <logger/logger.h>

namespace logger {

class LoggerPrivate {
public:

	LoggerPrivate()
	{}
};

Logger::~Logger()
{
	delete m_;
}

void Logger::sendMessage(LogMessageType type, const std::string& msg)
{
	std::cout << msg << std::endl;
}

Logger::Logger() :
	m_(new LoggerPrivate)
{
}

}
