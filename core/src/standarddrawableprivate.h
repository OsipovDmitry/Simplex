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
    StandardDrawablePrivate(std::shared_ptr<graphics::IVertexArray>);

    static glm::vec4 &defaultBaseColor();
    static float &defaultMetalness();
    static float &defaultRoughness();

private:
    static glm::vec4 s_defaultBaseColor;
    static float s_defaultMetalness;
    static float s_defaultRoughness;
};

}
}

#endif // CORE_STANDARDDRAWABLEPRIVATE_H
