#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(SpotLightNode)

public:
    SpotLightNodePrivate(SpotLightNode&, const std::string&);
    ~SpotLightNodePrivate() override;

    glm::vec2 &halfAngles();

private:
    glm::vec2 m_halfAngles;

    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

};

}
}

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
