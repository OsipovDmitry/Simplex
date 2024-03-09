#include "ibllightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> IBLLightNodePrivate::s_lightAreaVertexArray;
std::weak_ptr<const graphics::ITexture> IBLLightNodePrivate::s_defaultBRDFLutMap;
std::weak_ptr<const graphics::ITexture> IBLLightNodePrivate::s_defaultDiffuseMap;
std::weak_ptr<const graphics::ITexture> IBLLightNodePrivate::s_defaultSpecularMap;
float IBLLightNodePrivate::s_defaultContribution = .4f;

IBLLightNodePrivate::IBLLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

IBLLightNodePrivate::~IBLLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &IBLLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

std::weak_ptr<const graphics::ITexture> &IBLLightNodePrivate::defaultBRDFLutMap()
{
    return s_defaultBRDFLutMap;
}

std::weak_ptr<const graphics::ITexture> &IBLLightNodePrivate::defaultDiffuseMap()
{
    return s_defaultDiffuseMap;
}

std::weak_ptr<const graphics::ITexture> &IBLLightNodePrivate::defaultSpecularMap()
{
    return s_defaultSpecularMap;
}

float &IBLLightNodePrivate::defaultContribution()
{
    return s_defaultContribution;
}

}
}
