#ifndef UTILS_SETTINGS_H
#define UTILS_SETTINGS_H

#include <memory>
#include <filesystem>

#include <utils/rapidjson/document.h>
#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

class SettingsPrivate;
class UTILS_SHARED_EXPORT Settings
{
    NONCOPYBLE(Settings)
public:
    virtual ~Settings();

    static void initialize(const std::filesystem::path&);
    static const std::filesystem::path &path();

protected:
    explicit Settings();

    static const rapidjson::Document &documnet();

private:
    std::unique_ptr<SettingsPrivate> m_;
};

}
}

#endif // UTILS_SETTINGS_H
