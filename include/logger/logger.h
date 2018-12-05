#ifndef LOGGER_H
#define LOGGER_H

#include "../utils/singletoon.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"

#include "abstractlog.h"
#include "logger_global.h"

#define LOG_DEBUG(str) logger::Logger::instance().sendMessage(logger::LogMessageType::Debug, str)
#define LOG_INFO(str) logger::Logger::instance().sendMessage(logger::LogMessageType::Info, str)
#define LOG_WARNING(str) logger::Logger::instance().sendMessage(logger::LogMessageType::Warning, str)
#define LOG_ERROR(str) logger::Logger::instance().sendMessage(logger::LogMessageType::Error, str)
#define LOG_CRITICAL(str) logger::Logger::instance().sendMessage(logger::LogMessageType::Critical, str)

namespace logger {

class LoggerPrivate;
class LOGGERSHARED_EXPORT Logger
{
	SINGLETON(Logger)
    PIMPL(Logger)
    NONCOPYBLE(Logger)

public:
    void sendMessage(LogMessageType type, const std::string& msg);

private:
	Logger();
    ~Logger();

    LoggerPrivate *m_;
};

}

#endif // LOGGER_H
