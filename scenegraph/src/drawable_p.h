#ifndef DRAWABLE_P_H
#define DRAWABLE_P_H

#include <renderer/forwarddecl.h>

namespace scenegraph
{

class DrawablePrivate
{
public:
    renderer::ContextPtr context;
    renderer::VertexArrayPtr vao;

    DrawablePrivate(renderer::ContextPtr c) : context(c) {}
};

}

#endif // DRAWABLE_P_H
