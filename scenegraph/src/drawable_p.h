#ifndef DRAWABLE_P_H
#define DRAWABLE_P_H

#include <types/forwarddecl.h>
#include <renderer/forwarddecl.h>
#include <scenegraph/forwarddecl.h>

namespace scenegraph
{

class DrawablePrivate
{
public:
    renderer::VertexArrayPtr vao;
    MaterialPtr material;
    RenderStatePtr renderState;
    types::IndexType indicesType;
    types::PrimitiveType primitivesType;
    uint32_t numIndices;
    uint32_t indexBufferOffset;

    DrawablePrivate();

};

}

#endif // DRAWABLE_P_H
