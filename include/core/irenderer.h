#ifndef CORE_IRENDERER_H
#define CORE_IRENDERER_H

#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IRenderer : public INamedObject
{
public:
    ~IRenderer() override = default;;

    virtual bool makeCurrent() = 0;
    virtual bool doneCurrent() = 0;
};

}
}

#endif // CORE_IRENDERER_H
