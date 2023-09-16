#include <fstream>

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
    static auto readFile = [](const std::filesystem::path &filename, std::string &data) -> bool
    {
        data.clear();

        std::ifstream shaderFile(filename);
        if (!shaderFile.is_open())
            return false;

        shaderFile.seekg(0, std::ios::end);
        const auto size = shaderFile.tellg();
        data.resize(static_cast<std::string::size_type>(size));
        shaderFile.seekg(0, std::ios::beg);
        shaderFile.read(&data[0], size);
        shaderFile.close();

        return true;
    };

    std::string jsonData;
    if (SettingsPrivate::settingsFilename == "default")
        jsonData = "{}";
    else if (!readFile(SettingsPrivate::settingsFilename, jsonData))
    {
        assert(false);
    }

    m_->document.Parse(jsonData.c_str());
    if (!m_->document.IsObject())
    {
        assert(false);
    }
}

const rapidjson::Document &Settings::documnet()
{
    static Settings settings;
    return settings.m_->document;
}

}
}
