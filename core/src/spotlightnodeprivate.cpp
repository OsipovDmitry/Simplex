#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> SpotLightNodePrivate::s_lightAreaVertexArray;
utils::BoundingBox SpotLightNodePrivate::s_lightAreaBoundingBox;

SpotLightNodePrivate::SpotLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

glm::vec2 &SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
}

std::shared_ptr<ShadowFrameBuffer> SpotLightNodePrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBuffer2D>(graphicsRenderer);
}

const glm::mat4x4 &SpotLightNodePrivate::shadowBiasMatrix() const
{
    static const glm::mat4x4 s_biasMatrix = utils::Transform::fromTranslation(glm::vec3(.5f)) * utils::Transform::fromScale(.5f);
    return s_biasMatrix;
}

std::weak_ptr<graphics::IVertexArray> &SpotLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

utils::BoundingBox &SpotLightNodePrivate::lightAreaBoundingBox()
{
    return s_lightAreaBoundingBox;
}

}
}
