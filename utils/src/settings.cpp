#include <utils/logger.h>
#include <utils/textfile.h>
#include <utils/settings.h>

namespace simplex
{
namespace utils
{

class SettingsPrivate
{
public:
    SettingsPrivate()
    {
    }

    ~SettingsPrivate() = default;

    rapidjson::Document document;
    static std::filesystem::path settingsFilename;
};

std::filesystem::path SettingsPrivate::settingsFilename = "default";

Settings::~Settings() = default;

void Settings::initialize(const std::filesystem::path &settingsFilename)
{
    SettingsPrivate::settingsFilename = settingsFilename;
}

const std::filesystem::path &Settings::path()
{
    return SettingsPrivate::settingsFilename;
}

Settings::Settings()
    : m_(std::make_unique<SettingsPrivate>())
{
    std::string jsonData;
    if (SettingsPrivate::settingsFilename == "default")
        jsonData = "{}";
    else
    {
        auto absoluteFilename = std::filesystem::absolute(SettingsPrivate::settingsFilename);

        if (auto settingsFile = TextFile::loadFromFile(absoluteFilename); settingsFile)
            jsonData = std::move(settingsFile->data());
        else
            LOG_CRITICAL << "Can't open settings file \"" << absoluteFilename << "\"";
    }

    m_->document.Parse(jsonData.c_str());
    if (!m_->document.IsObject())
    {
        LOG_CRITICAL << "Settings file \"" << std::filesystem::absolute(SettingsPrivate::settingsFilename) << "\" is damaged";
    }
}

const rapidjson::Document &Settings::documnet()
{
    static Settings settings;
    return settings.m_->document;
}

}
}
