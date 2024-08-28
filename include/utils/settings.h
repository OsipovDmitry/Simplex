#ifndef UTILS_SETTINGS_H
#define UTILS_SETTINGS_H

#include <filesystem>

#include <utils/rapidjson/document.h>
#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT SettingsComponent
{
    NONCOPYBLE(SettingsComponent)
public:
    virtual ~SettingsComponent();

protected:
    SettingsComponent(const rapidjson::Document::ValueType*);

    const rapidjson::Document::ValueType *value() const;
    void setValue(const rapidjson::Document::ValueType*);

    const rapidjson::Document::ValueType *read(const std::string &) const;
    std::string readString(const std::string&, const std::string& = "") const;
    bool readBool(const std::string&, bool = false) const;
    float readSingle(const std::string&, float = 0.f) const;
    glm::vec2 readVec2(const std::string&, const glm::vec2& = glm::vec2(0.f)) const;
    glm::vec3 readVec3(const std::string&, const glm::vec3& = glm::vec3(0.f)) const;
    glm::vec4 readVec4(const std::string&, const glm::vec4& = glm::vec4(0.f)) const;
    int32_t readInt(const std::string&, int32_t = 0) const;
    glm::i32vec2 readIVec2(const std::string&, const glm::i32vec2& = glm::i32vec2(0)) const;
    glm::i32vec3 readIVec3(const std::string&, const glm::i32vec3& = glm::i32vec3(0)) const;
    glm::i32vec4 readIVec4(const std::string&, const glm::i32vec4& = glm::i32vec4(0)) const;
    uint32_t readUint(const std::string&, uint32_t = 0u) const;
    glm::u32vec2 readUVec2(const std::string&, const glm::u32vec2& = glm::u32vec2(0u)) const;
    glm::u32vec3 readUVec3(const std::string&, const glm::u32vec3& = glm::u32vec3(0u)) const;
    glm::u32vec4 readUVec4(const std::string&, const glm::u32vec4& = glm::u32vec4(0u)) const;

private:
    const rapidjson::Document::ValueType *m_value;
};

class UTILS_SHARED_EXPORT Settings : public SettingsComponent
{
public:
    ~Settings() override;

protected:
    Settings(const std::filesystem::path&);

    const rapidjson::Document &documnet();
    const std::filesystem::path &path();

private:
    rapidjson::Document m_document;
    std::filesystem::path m_path;
};

}
}

#endif // UTILS_SETTINGS_H
