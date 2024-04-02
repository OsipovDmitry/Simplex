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

std::shared_ptr<ShadowFrameBuffer> IBLLightNodePrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &) const
{
    return nullptr;
}

const glm::mat4x4 &IBLLightNodePrivate::shadowBiasMatrix() const
{
    static const glm::mat4x4 s_biasMatrix(1.f);
    return s_biasMatrix;
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
