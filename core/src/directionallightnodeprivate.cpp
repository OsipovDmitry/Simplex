#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> DirectionalLightNodePrivate::s_lightAreaVertexArray;

DirectionalLightNodePrivate::DirectionalLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

std::shared_ptr<ShadowFrameBuffer> DirectionalLightNodePrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBuffer2D>(graphicsRenderer);
}

const glm::mat4x4 &DirectionalLightNodePrivate::shadowBiasMatrix() const
{
    static const glm::mat4x4 s_biasMatrix = utils::Transform::fromTranslation(glm::vec3(.5f)) * utils::Transform::fromScale(.5f);
    return s_biasMatrix;
}

std::weak_ptr<graphics::IVertexArray> &DirectionalLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

}
}
