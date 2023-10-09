#ifndef LIGHTDRAWABLEPRIVATE_H
#define LIGHTDRAWABLEPRIVATE_H

#include <core/forwarddecl.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

class LightDrawablePrivate : public DrawablePrivate
{
public:
    LightDrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&, LightDrawableType);
    ~LightDrawablePrivate() override;

    LightDrawableType &type();

private:
    LightDrawableType m_type;
};

}
}

#endif // LIGHTDRAWABLEPRIVATE_H
