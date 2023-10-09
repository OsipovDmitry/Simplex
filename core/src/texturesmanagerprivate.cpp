#include <utils/image.h>

#include <core/igraphicsrenderer.h>

#include "texturesmanagerprivate.h"

namespace simplex
{
namespace core
{

const std::string TexturesManagerPrivate::s_defaultIBLEnvironmentTextureName = "DefaultIBLEnvironment";
const std::string TexturesManagerPrivate::s_defaultIBLBRDFLutTextureName = "DefaultIBLBRDFLut";
const std::string TexturesManagerPrivate::s_defaultIBLDiffuseTextureName = "DefaultIBLDiffuse";
const std::string TexturesManagerPrivate::s_defaultIBLSpecularTextureName = "DefaultIBLSpecular";

const std::filesystem::path TexturesManagerPrivate::s_defaultIBLEnvironmentTexturePath = "D:/res/textures/ibl/environment.json";
const std::filesystem::path TexturesManagerPrivate::s_defaultIBLBRDFLutTexturePath = "D:/res/textures/ibl/brdf.json";
const std::filesystem::path TexturesManagerPrivate::s_defaultIBLDiffuseTexturePath = "D:/res/textures/ibl/irradiance.json";
const std::filesystem::path TexturesManagerPrivate::s_defaultIBLSpecularTexturePath = "D:/res/textures/ibl/radiance.json";

TexturesManagerPrivate::TexturesManagerPrivate(std::shared_ptr<graphics::IRenderer> renderer)
    : m_renderer(renderer)
{
}

TexturesManagerPrivate::~TexturesManagerPrivate() = default;

std::shared_ptr<graphics::IRenderer> &TexturesManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> &TexturesManagerPrivate::resources()
{
    return m_resources;
}

const std::string &TexturesManagerPrivate::defaultIBLEnvironmentTextureName()
{
    return s_defaultIBLEnvironmentTextureName;
}

const std::string &TexturesManagerPrivate::defaultIBLBRDFLutTextureName()
{
    return s_defaultIBLBRDFLutTextureName;
}

const std::string &TexturesManagerPrivate::defaultIBLDiffuseTextureName()
{
    return s_defaultIBLDiffuseTextureName;
}

const std::string &TexturesManagerPrivate::defaultIBLSpecularTextureName()
{
    return s_defaultIBLSpecularTextureName;
}

const std::filesystem::path &TexturesManagerPrivate::defaultIBLEnvironmentTexturePath()
{
    return s_defaultIBLEnvironmentTexturePath;
}

const std::filesystem::path &TexturesManagerPrivate::defaultIBLBRDFLutTexturePath()
{
    return s_defaultIBLBRDFLutTexturePath;
}

const std::filesystem::path &TexturesManagerPrivate::defaultIBLDiffuseTexturePath()
{
    return s_defaultIBLDiffuseTexturePath;
}

const std::filesystem::path &TexturesManagerPrivate::defaultIBLSpecularTexturePath()
{
    return s_defaultIBLSpecularTexturePath;
}

}
}
