#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate : public LightNodePrivate
{
public:
    DirectionalLightNodePrivate(DirectionalLightNode&, const std::string&);
    ~DirectionalLightNodePrivate() override;

    void doAfterTransformChanged() override;

private:
    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;
};

}
}

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
