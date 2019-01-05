#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "scenegraph_global.h"
#include "forwarddecl.h"
#include "../renderer/forwarddecl.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace scenegraph
{

class DrawablePrivate;
class SCENEGRAPHSHARED_EXPORT Drawable {
    PIMPL(Drawable)
    NONCOPYBLE(Drawable)
    CUSTOMDELETER(Drawable)

public:
    renderer::ContextPtr context() const;

    renderer::VertexArrayPtr vertexArray() const;
    void setVertexArray(renderer::VertexArrayPtr value);

    static DrawablePtr create(renderer::ContextPtr context);

private:
    Drawable(renderer::ContextPtr context);
    ~Drawable();

    DrawablePrivate *m_;
}


}

#endif // DRAWABLE_H
