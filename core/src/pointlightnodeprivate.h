#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(PointLightNode)

public:
    PointLightNodePrivate(PointLightNode&, const std::string&);
    ~PointLightNodePrivate() override;

private:
    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

};

}
}

#endif // CORE_POINTLIGHTNODEPRIVATE_H
