#ifndef CORE_TEXTURESMANAGERPRIVATE_H
#define CORE_TEXTURESMANAGERPRIVATE_H

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <string>

#include <utils/forwarddecl.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class TexturesManagerPrivate
{
public:
    TexturesManagerPrivate(std::shared_ptr<graphics::IRenderer>);
    ~TexturesManagerPrivate();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> &resources();

    static const std::string &defaultIBLEnvironmentTextureName();
    static const std::string &defaultIBLBRDFLutTextureName();
    static const std::string &defaultIBLDiffuseTextureName();
    static const std::string &defaultIBLSpecularTextureName();

    static const std::filesystem::path &defaultIBLEnvironmentTexturePath();
    static const std::filesystem::path &defaultIBLBRDFLutTexturePath();
    static const std::filesystem::path &defaultIBLDiffuseTexturePath();
    static const std::filesystem::path &defaultIBLSpecularTexturePath();

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> m_resources;

    static const std::string s_defaultIBLEnvironmentTextureName;
    static const std::string s_defaultIBLBRDFLutTextureName;
    static const std::string s_defaultIBLDiffuseTextureName;
    static const std::string s_defaultIBLSpecularTextureName;

    static const std::filesystem::path s_defaultIBLEnvironmentTexturePath;
    static const std::filesystem::path s_defaultIBLBRDFLutTexturePath;
    static const std::filesystem::path s_defaultIBLDiffuseTexturePath;
    static const std::filesystem::path s_defaultIBLSpecularTexturePath;

};

}
}

#endif // CORE_TEXTURESMANAGERPRIVATE_H
