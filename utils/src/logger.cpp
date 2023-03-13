#include <sstream>
#include <fstream>
#include <array>
#include <ctime>

#include <utils/logger.h>

namespace simplex
{
namespace utils
{

class LoggerPrivate
{
public:
    LoggerPrivate(Logger::MessageLevel messageLevel, const std::string &file, int line)
        : m_stringStream()
        , m_messageLevel(messageLevel)
        , m_file(file)
        , m_line(line)
    {
    }

    std::stringstream &stringStream() { return m_stringStream; }
    Logger::MessageLevel messageLevel() const { return m_messageLevel; }
    const std::string &file() const { return m_file; }
    int line() const { return m_line; }

    static void setLoggerOutput(std::shared_ptr<Logger::Output> output) { s_output = output; }
    static std::shared_ptr<Logger::Output> loggerOutput() { return s_output; }

    static void setLoggerMinMessageLevel(Logger::MessageLevel value) { s_minMessageLevel = value; }
    static Logger::MessageLevel loggerMinMessageLevel() { return s_minMessageLevel; }

    static std::string currentDateTimeString() {
        std::time_t time = std::time(nullptr);
        std::string result(100, ' ');
        result.resize(std::strftime(result.data(), result.length(), "%d-%m-%y %H.%M.%S", std::localtime(&time)));
        return result;
    }

private:
    std::stringstream m_stringStream;
    Logger::MessageLevel m_messageLevel;
    std::string m_file;
    int m_line;
    static std::shared_ptr<Logger::Output> s_output;
    static Logger::MessageLevel s_minMessageLevel;
};

std::shared_ptr<Logger::Output> LoggerPrivate::s_output = std::make_shared<Logger::FileOutput>(
            "Simplex3DEngine" + LoggerPrivate::currentDateTimeString() + ".txt"
);
Logger::MessageLevel LoggerPrivate::s_minMessageLevel = Logger::MessageLevel::Info;

class Logger::FileOutputPrivate
{
public:
    FileOutputPrivate(const std::string& fileOutputPrefix)
        : m_fileOuputName(fileOutputPrefix)
    {}

    const std::string &fileOutputName() const { return m_fileOuputName; }

private:
    std::string m_fileOuputName;
};

Logger::Logger(MessageLevel messageLevel, const std::string &file, int line)
    : m_(std::make_unique<LoggerPrivate>(messageLevel, file, line))
{
}

Logger::~Logger()
{   
    static std::array<std::string, numElementsMessageLevel()> s_messageTypesNames {
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };

    if (auto output = LoggerPrivate::loggerOutput(); output != nullptr)
    {
        output->operator << (
                LoggerPrivate::currentDateTimeString() +
                (m_->file().empty() ? "" : " " + m_->file()) +
                (m_->line() == -1 ? "" : ":" + std::to_string(m_->line())) +
                " [" +
                s_messageTypesNames[castFromMessageLevel(m_->messageLevel())] +
                "]: " +
                m_->stringStream().str()
        );
    }
}

void Logger::setOutput(std::shared_ptr<Output> output)
{
    LoggerPrivate::setLoggerOutput(output);
}

std::shared_ptr<Logger::Output> Logger::output()
{
    return LoggerPrivate::loggerOutput();
}

void Logger::setMinMessageLevel(MessageLevel value)
{
    LoggerPrivate::setLoggerMinMessageLevel(value);
}

Logger::MessageLevel Logger::minMessageLevel()
{
    return LoggerPrivate::loggerMinMessageLevel();
}

Logger &Logger::operator <<(const char *value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(const std::string &value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(bool value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(float value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(double value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(int8_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(uint8_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(int16_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(uint16_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(int32_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(uint32_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(int64_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger &Logger::operator <<(uint64_t value)
{
    if (m_->messageLevel() >= LoggerPrivate::loggerMinMessageLevel())
        m_->stringStream() << value;
    return *this;
}

Logger::Output::Output()
{
}

Logger::Output::~Output()
{
}

Logger::FileOutput::FileOutput(const std::string& fileOutputPrefix)
    : Output()
    , m_(std::make_unique<Logger::FileOutputPrivate>(fileOutputPrefix))
{
}

Logger::FileOutput::~FileOutput()
{

}

void Logger::FileOutput::operator <<(const std::string &message)
{
    std::ofstream file;
    file.open(m_->fileOutputName().c_str(), std::ios::out | std::ios::app);
    if (file.is_open())
    {
        file << message << std::endl;
        file.close();
    }
}

}
}
