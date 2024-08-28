#ifndef CORE_IBLLIGHTNODEPRIVATE_H
#define CORE_IBLLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class IBLLightNodePrivate : public LightNodePrivate
{
public:
    IBLLightNodePrivate(IBLLightNode&, const std::string &name);
    ~IBLLightNodePrivate() override;

    void doAfterTransformChanged() override;

private:
    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

};

}
}

#endif // CORE_IBLLIGHTNODEPRIVATE_H
