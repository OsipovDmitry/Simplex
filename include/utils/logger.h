#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <memory>
#include <string>
#include <filesystem>

#include <utils/noncopyble.h>
#include <utils/enumclass.h>
#include <utils/utilsglobal.h>

#define LOG_INFO simplex::utils::Logger(simplex::utils::Logger::MessageLevel::Info, __FILE__, __LINE__)
#define LOG_WARNING simplex::utils::Logger(simplex::utils::Logger::MessageLevel::Warning, __FILE__, __LINE__)
#define LOG_ERROR simplex::utils::Logger(simplex::utils::Logger::MessageLevel::Error, __FILE__, __LINE__)
#define LOG_CRITICAL simplex::utils::Logger(simplex::utils::Logger::MessageLevel::Critical, __FILE__, __LINE__)

namespace simplex
{
namespace utils
{

class LoggerPrivate;
class UTILS_SHARED_EXPORT Logger
{
    NONCOPYBLE(Logger)
public:
    class UTILS_SHARED_EXPORT Output
    {
        NONCOPYBLE(Output)
    public:
        Output();
        virtual ~Output();
        virtual void operator <<(const std::string&) = 0;
    };

    class FileOutputPrivate;
    class UTILS_SHARED_EXPORT FileOutput : public Output
    {
        NONCOPYBLE(FileOutput)
    public:
        FileOutput(const std::string&);
        ~FileOutput() override;
        void operator <<(const std::string&) override;

    private:
        std::unique_ptr<FileOutputPrivate> m_;
    };

    static void setOutput(std::shared_ptr<Output>);
    static std::shared_ptr<Output> output();

    ENUMCLASS(MessageLevel, std::size_t, Info, Warning, Error, Critical)
    static void setMinMessageLevel(MessageLevel);
    static MessageLevel minMessageLevel();

    Logger(MessageLevel, const std::string &file = "", int line = -1);
    ~Logger();

    Logger &operator <<(const char*);
    Logger &operator <<(const std::string&);
    Logger &operator <<(const std::filesystem::path&);
    Logger &operator <<(bool);
    Logger &operator <<(float);
    Logger &operator <<(double);
    Logger &operator <<(int8_t);
    Logger &operator <<(uint8_t);
    Logger &operator <<(int16_t);
    Logger &operator <<(uint16_t);
    Logger &operator <<(int32_t);
    Logger &operator <<(uint32_t);
    Logger &operator <<(int64_t);
    Logger &operator <<(uint64_t);

private:
    std::unique_ptr<LoggerPrivate> m_;
};

}
}

#endif // UTILS_LOGGER_H
