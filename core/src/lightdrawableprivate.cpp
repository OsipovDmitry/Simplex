#include "lightdrawableprivate.h"

namespace simplex
{
namespace core
{

LightDrawablePrivate::LightDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao, LightDrawableType type)
    : DrawablePrivate(vao)
    , m_type(type)
{
}

LightDrawablePrivate::~LightDrawablePrivate() = default;

LightDrawableType &LightDrawablePrivate::type()
{
    return m_type;
}

}
}
