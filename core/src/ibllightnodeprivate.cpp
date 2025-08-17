#include <core/ibllightnode.h>

#include "ibllightnodeprivate.h"

namespace simplex
{
namespace core
{

IBLLightNodePrivate::IBLLightNodePrivate(IBLLightNode &iblLightNode, const std::string &name)
    : LightNodePrivate(iblLightNode, name, LightType::IBL)
{
}

IBLLightNodePrivate::~IBLLightNodePrivate() = default;

void IBLLightNodePrivate::doAfterTransformChanged()
{
    dirtyAreaMatrix();
    dirtyAreaBoundingBox();
}

glm::mat4x4 IBLLightNodePrivate::doAreaMatrix()
{
    return glm::mat4x4(1.f); // it is not used because bb policy is Root
}

utils::BoundingBox IBLLightNodePrivate::doAreaBoundingBox()
{
    return utils::BoundingBox::empty(); // it is not used because bb policy is Root
}

}
}
