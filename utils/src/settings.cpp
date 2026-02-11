#include <utils/glm/common.hpp>
#include <utils/epsilon.h>
#include <utils/logger.h>
#include <utils/textfile.h>
#include <utils/settings.h>

namespace simplex
{
namespace utils
{

inline float readSingleImpl(const rapidjson::Document::ValueType *value, float defaultValue)
{
    using T = decltype(defaultValue);
    T result = defaultValue;

    if (value->IsFloat())
        result = value->GetFloat();
    else if (value->IsString())
    {
        std::string valueString = value->GetString();
        for (auto &c : valueString)
            c = static_cast<char>(std::tolower(c));

        T sign = static_cast<T>(1);
        if (!valueString.empty() && valueString[0] == '-')
        {
            sign = static_cast<T>(-1);
            valueString = valueString.substr(1u);
        }

        if (valueString == "max")
            result = utils::maximum<T>();
        else if (valueString == "eps")
            result = utils::epsilon<T>();

        result *= sign;
    }

    return result;
}

template <glm::length_t L, typename T = glm::vec<L, float>>
inline T readVecImpl(const rapidjson::Document::ValueType *value, const T &defaultValue)
{
    T result = defaultValue;

    if (value->IsArray())
    {
        auto array = value->GetArray();
        const auto size = glm::min(static_cast<rapidjson::SizeType>(L), array.Size());
        for (rapidjson::SizeType i = 0; i < size; ++i)
            result[static_cast<glm::length_t>(i)] = readSingleImpl(&array[i], defaultValue[static_cast<glm::length_t>(i)]);
    }

    return result;
}

inline int32_t readIntImpl(const rapidjson::Document::ValueType *value, int32_t defaultValue)
{
    using T = decltype(defaultValue);
    T result = defaultValue;

    if (value->IsInt())
        result = value->GetInt();
    else if (value->IsString())
    {
        std::string valueString = value->GetString();
        for (auto &c : valueString)
            c = static_cast<char>(std::tolower(c));

        T sign = static_cast<T>(1);
        if (!valueString.empty() && valueString[0] == '-')
        {
            sign = static_cast<T>(-1);
            valueString = valueString.substr(1u);
        }

        if (valueString == "max")
            result = utils::maximum<T>();

        result *= sign;
    }

    return result;
}

template <glm::length_t L, typename T = glm::vec<L, int32_t>>
inline T readIVecImpl(const rapidjson::Document::ValueType *value, const T &defaultValue)
{
    T result = defaultValue;

    if (value->IsArray())
    {
        auto array = value->GetArray();
        const auto size = glm::min(static_cast<rapidjson::SizeType>(L), array.Size());
        for (rapidjson::SizeType i = 0; i < size; ++i)
            result[static_cast<glm::length_t>(i)] = readIntImpl(&array[i], defaultValue[static_cast<glm::length_t>(i)]);
    }

    return result;
}

inline uint32_t readUintImpl(const rapidjson::Document::ValueType *value, uint32_t defaultValue)
{
    using T = decltype(defaultValue);
    T result = defaultValue;

    if (value->IsUint())
        result = value->GetUint();
    else if (value->IsString())
    {
        std::string valueString = value->GetString();
        for (auto &c : valueString)
            c = static_cast<char>(std::tolower(c));

        if (valueString == "max")
            result = utils::maximum<T>();
    }

    return result;
}

template <glm::length_t L, typename T = glm::vec<L, uint32_t>>
inline T readUVecImpl(const rapidjson::Document::ValueType *value, const T &defaultValue)
{
    T result = defaultValue;

    if (value->IsArray())
    {
        auto array = value->GetArray();
        const auto size = glm::min(static_cast<rapidjson::SizeType>(L), array.Size());
        for (rapidjson::SizeType i = 0; i < size; ++i)
            result[static_cast<glm::length_t>(i)] = readUintImpl(&array[i], defaultValue[static_cast<glm::length_t>(i)]);
    }

    return result;
}

SettingsComponent::~SettingsComponent() = default;

SettingsComponent::SettingsComponent(const rapidjson::Document::ValueType *value)
    : m_value(value)
{
}

const rapidjson::Document::ValueType *SettingsComponent::value() const
{
    return m_value;
}

void SettingsComponent::setValue(const rapidjson::Document::ValueType *value)
{
    m_value = value;
}

const rapidjson::Document::ValueType* SettingsComponent::read(const std::string &path) const
{
    static const auto oneStep = [](
            const rapidjson::Document::ValueType *curDoc,
            const std::string& path,
            size_t& current,
            size_t& previous) -> const rapidjson::Document::ValueType*
    {
        const auto member = path.substr(previous, current - previous);

        if (!curDoc->HasMember(member.c_str()))
            return nullptr;

        previous = current + 1u;
        current = path.find('.', previous);
        return &(curDoc->operator [](member.c_str()));
    };

    const rapidjson::Document::ValueType *curDocument = m_value;
    size_t current = path.find('.'), previous = 0;

    while (current != std::string::npos && curDocument)
        curDocument = oneStep(curDocument, path, current, previous);

    if (curDocument)
        curDocument = oneStep(curDocument, path, current, previous);

    if (!curDocument)
        LOG_ERROR << "Settings document doesn't have member \"" << path.substr(previous, current - previous) << "\"";

    return curDocument;
}

std::string SettingsComponent::readString(const std::string &path, const std::string &defaultValue) const
{
    auto obj = read(path);
    return (obj && obj->IsString()) ? obj->GetString() : defaultValue;
}

bool SettingsComponent::readBool(const std::string &path, bool defaultValue) const
{
    auto obj = read(path);
    return (obj && obj->IsBool()) ? obj->GetBool() : defaultValue;
}

float SettingsComponent::readSingle(const std::string &path, float defaultValue) const
{
    auto obj = read(path);
    return obj ? readSingleImpl(obj, defaultValue) : defaultValue;
}

glm::vec2 SettingsComponent::readVec2(const std::string &path, const glm::vec2 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::vec3 SettingsComponent::readVec3(const std::string &path, const glm::vec3 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::vec4 SettingsComponent::readVec4(const std::string &path, const glm::vec4 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

int32_t SettingsComponent::readInt(const std::string &path, int32_t defaultValue) const
{
    auto obj = read(path);
    return obj ? readIntImpl(obj, defaultValue) : defaultValue;
}

glm::i32vec2 SettingsComponent::readIVec2(const std::string &path, const glm::i32vec2 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readIVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::i32vec3 SettingsComponent::readIVec3(const std::string &path, const glm::i32vec3 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readIVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::i32vec4 SettingsComponent::readIVec4(const std::string &path, const glm::i32vec4 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readIVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

uint32_t SettingsComponent::readUint(const std::string &path, uint32_t defaultValue) const
{
    auto obj = read(path);
    return obj ? readUintImpl(obj, defaultValue) : defaultValue;
}

glm::u32vec2 SettingsComponent::readUVec2(const std::string &path, const glm::u32vec2 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readUVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::u32vec3 SettingsComponent::readUVec3(const std::string &path, const glm::u32vec3 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readUVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

glm::u32vec4 SettingsComponent::readUVec4(const std::string &path, const glm::u32vec4 &defaultValue) const
{
    auto obj = read(path);
    return obj ? readUVecImpl<std::remove_reference_t<decltype(defaultValue)>::length()>(obj, defaultValue) : defaultValue;
}

Settings::~Settings() = default;

const rapidjson::Document &Settings::documnet()
{
    return m_document;
}

const std::filesystem::path &Settings::path()
{
    return m_path;
}

Settings::Settings(const std::filesystem::path &path)
    : SettingsComponent(nullptr)
    , m_document()
    , m_path(path)
{
    auto absoluteFilename = std::filesystem::canonical(m_path);

    if (auto settingsFile = TextFile::loadFromFile(absoluteFilename); settingsFile)
        m_document.Parse(settingsFile->data().c_str());
    else
        LOG_CRITICAL << "Can't open settings file \"" << absoluteFilename << "\"";

    if (!m_document.IsObject())
        LOG_CRITICAL << "Settings file \"" << absoluteFilename << "\" is damaged";

    setValue(&m_document);
}

}
}
