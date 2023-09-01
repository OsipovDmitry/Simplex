#ifndef CORE_STANDARDDRAWABLEPRIVATE_H
#define CORE_STANDARDDRAWABLEPRIVATE_H

#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

class StandardDrawablePrivate : public DrawableBasePrivate
{
public:
    StandardDrawablePrivate(std::shared_ptr<graphics::IVertexArray>);

protected:

};

}
}

#endif // CORE_STANDARDDRAWABLEPRIVATE_H
