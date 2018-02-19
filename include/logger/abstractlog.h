#ifndef ABSTRACTLOG_H
#define ABSTRACTLOG_H

#include <string>

namespace logger {

enum class LogMessageType {
	Debug = 0,
	Info,
	Warning,
	Error,
	Critical,
	Count
};

//class AbstractLogPrivate;

//class AbstractLog {
//public:
//	void printMessage(LogMessageType type, const std::string& msg, const std::string& time);
//	void setFilter(LogMessageType filterValue);

//	static std::string LogMessageTypeToString(LogMessageType type);

//protected:
//	AbstractLog();
//	virtual ~AbstractLog();

//	virtual void print(LogMessageType type, const std::string& msg, const std::string& time) = 0;

//private:
//	std::unique_ptr<AbstractLogPrivate> m;

//};

}

#endif // ABSTRACTLOG_H
