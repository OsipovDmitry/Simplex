#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> PointLightNodePrivate::s_lightAreaVertexArray;
utils::BoundingBox PointLightNodePrivate::s_lightAreaBoundingBox;

PointLightNodePrivate::PointLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

std::shared_ptr<ShadowFrameBuffer> PointLightNodePrivate::createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer) const
{
    return std::make_shared<ShadowFrameBufferCube>(graphicsRenderer);
}

const glm::mat4x4 &PointLightNodePrivate::shadowBiasMatrix() const
{
    static const glm::mat4x4 s_biasMatrix(1.f);
    return s_biasMatrix;
}

std::weak_ptr<graphics::IVertexArray> &PointLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

utils::BoundingBox &PointLightNodePrivate::lightAreaBoundingBox()
{
    return s_lightAreaBoundingBox;
}

}
}
