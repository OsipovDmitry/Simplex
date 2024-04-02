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
    LightDrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&);
    ~LightDrawablePrivate() override;  

};

}
}

#endif // LIGHTDRAWABLEPRIVATE_H
