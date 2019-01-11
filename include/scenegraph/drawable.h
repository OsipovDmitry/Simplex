#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "scenegraph_global.h"
#include "forwarddecl.h"
#include "../renderer/forwarddecl.h"
#include "../types/forwarddecl.h"
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
    void setGeometry(renderer::VertexArrayPtr vao, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, uint32_t indexBufferOffset = 0);
    renderer::VertexArrayPtr vertexArray() const;
    types::PrimitiveType primitiveType() const;
    uint32_t numIndices() const;
    types::IndexType indicesType() const;
    uint32_t indexBufferOffset() const;

    MaterialPtr material() const;
    void setMaterial(MaterialPtr value);

    const RenderState &renderState() const;
    RenderState& renderState();

    static DrawablePtr create();

private:
    Drawable();
    ~Drawable();

    DrawablePrivate *m_;
};


}

#endif // DRAWABLE_H
