#ifndef CORE_BACKGROUNDDRAWABLEPRIVATE_H
#define CORE_BACKGROUNDDRAWABLEPRIVATE_H

#include <utils/forwarddecl.h>

#include <core/forwarddecl.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

class BackgroundDrawablePrivate : public DrawablePrivate
{
public:
    BackgroundDrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&);
    ~BackgroundDrawablePrivate() override;

    static std::weak_ptr<graphics::IVertexArray> &screenQuadVertexArray();

    static std::weak_ptr<const graphics::ITexture> &defaultColorMap();
    static glm::vec3 &defaultBaseColor();
    static float &defaultRoughness();

private:
    static std::weak_ptr<graphics::IVertexArray> s_screenQuadVertexArray;

    static std::weak_ptr<const graphics::ITexture> s_defaultColorMap;
    static glm::vec3 s_defaultBaseColor;
    static float s_defaultRoughness;
};

}
}

#endif // CORE_BACKGROUNDDRAWABLEPRIVATE_H
