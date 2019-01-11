#include <types/geometry.h>
#include <scenegraph/renderstate.h>

#include "drawable_p.h"

namespace scenegraph
{

DrawablePrivate::DrawablePrivate() :
    renderState(RenderState::create()),
    indicesType(types::IndexType::Count),
    primitivesType(types::PrimitiveType::Count),
    numIndices(0u),
    indexBufferOffset(0u)
{

}



}
