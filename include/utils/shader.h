#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>

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

    const std::string &data() const;

    static const std::string &version();
    static void setVersion(const std::string&);

    static bool printDebugShaders();
    static void setPrintDebugShaders(bool);

    static std::shared_ptr<Shader> loadFromData(const std::string&);
    static std::shared_ptr<Shader> loadFromFile(const std::filesystem::path&, const ShaderDefines& = {});

private:
    std::string m_data;
    static std::string s_version;
    static bool s_printDebugShaders;

};

}
}

#endif // UTILS_SHADER_H
