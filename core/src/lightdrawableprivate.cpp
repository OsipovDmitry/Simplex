#include "lightdrawableprivate.h"

namespace simplex
{
namespace core
{

LightDrawablePrivate::LightDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : DrawablePrivate(vao)
{
}

LightDrawablePrivate::~LightDrawablePrivate() = default;

}
}
