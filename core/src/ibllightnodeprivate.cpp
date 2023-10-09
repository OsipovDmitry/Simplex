#include "ibllightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> IBLLightNodePrivate::s_lightAreaVertexArray;
std::weak_ptr<const graphics::ITexture> IBLLightNodePrivate::s_defaultDiffuseTexture;
std::weak_ptr<const graphics::ITexture> IBLLightNodePrivate::s_defaultSpecularTexture;

IBLLightNodePrivate::IBLLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

IBLLightNodePrivate::~IBLLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &IBLLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

std::weak_ptr<const graphics::ITexture> &IBLLightNodePrivate::defaultDiffuseTexture()
{
    return s_defaultDiffuseTexture;
}

std::weak_ptr<const graphics::ITexture> &IBLLightNodePrivate::defaultSpecularTexture()
{
    return s_defaultSpecularTexture;
}

}
}
