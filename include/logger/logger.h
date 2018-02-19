#ifndef LOGGER_H
#define LOGGER_H

#include "../utils/singletoon.h"

#include "abstractlog.h"
#include "logger_global.h"

#define LOG_DEBUG(str) logger::Logger::instance()->sendMessage(logger::LogMessageType::Debug, str)
#define LOG_INFO(str) logger::Logger::instance()->sendMessage(logger::LogMessageType::Info, str)
#define LOG_WARNING(str) logger::Logger::instance()->sendMessage(logger::LogMessageType::Warning, str)
#define LOG_ERROR(str) logger::Logger::instance()->sendMessage(logger::LogMessageType::Error, str)
#define LOG_CRITICAL(str) logger::Logger::instance()->sendMessage(logger::LogMessageType::Critical, str)

namespace logger {

class LoggerPrivate;
class LOGGERSHARED_EXPORT Logger
{
	SINGLETON(Logger)
public:
	~Logger();

	void sendMessage(LogMessageType type, const std::string& msg);

private:
	Logger();

	LoggerPrivate *m;
};

}

#endif // LOGGER_H
