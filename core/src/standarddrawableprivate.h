#ifndef CORE_STANDARDDRAWABLEPRIVATE_H
#define CORE_STANDARDDRAWABLEPRIVATE_H

#include <utils/glm/vec4.hpp>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

class StandardDrawablePrivate : public DrawablePrivate
{
public:
    StandardDrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);

    utils::BoundingBox &boundingBox();

    static glm::u32vec4 &defaultORMSwizzleMask();
    static glm::vec4 &defaultBaseColor();
    static float &defaultMetalness();
    static float &defaultRoughness();
    static float &defaultNormalMapScale();
    static float &defaultOcclusionMapStrength();

private:
    utils::BoundingBox m_boundingBox;

    static glm::u32vec4 s_defaultORMSwizzleMask;
    static glm::vec4 s_defaultBaseColor;
    static float s_defaultMetalness;
    static float s_defaultRoughness;
    static float s_defaultNormalMapScale;
    static float s_defaultOcclusionMapStrength;
};

}
}

#endif // CORE_STANDARDDRAWABLEPRIVATE_H
