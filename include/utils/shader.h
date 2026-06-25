#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_set>

#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>
#include <utils/utilsglobal.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT Shader final
{
    NONCOPYBLE(Shader)
public:
    Shader();
    ~Shader();

    const std::string& data() const;

    static const std::string& version();
    static void setVersion(const std::string&);

    static std::unordered_set<std::string> extensions();
    static void setExtensions(const std::unordered_set<std::string>&);

    static bool printDebugShaders();
    static void setPrintDebugShaders(bool);

    static std::shared_ptr<Shader> loadFromData(const std::string&);
    static std::shared_ptr<Shader> loadFromFile(const std::filesystem::path&, const ShaderDefines& = {});

private:
    std::string m_data;
    static std::string s_version;
    static std::unordered_set<std::string> s_extensions;
    static bool s_printDebugShaders;
};

} // namespace utils
} // namespace simplex

#endif // UTILS_SHADER_H
