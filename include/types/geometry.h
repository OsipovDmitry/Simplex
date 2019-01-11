#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <memory>
#include <vector>
#include <map>

#include <utils/enumclass.h>

#include "forwarddecl.h"

namespace types
{

ENUMCLASS(PrimitiveType, int32_t,
    Points,
    LineStrip,
    LineLoop,
    Lines,
    TriangleStrip,
    TriangleFan,
    Triangles
)

ENUMCLASS(IndexType, int32_t,
    Uint8,
    Uint16,
    Uint32
)

ENUMCLASS(VertexAttributeType, int32_t,
    Position,
    Tangent,
    Binormal,
    Normal,
    TexCoord0,
    TexCoord1,
    TexCoord2,
    TexCoord3
)

struct VertexArray
{
    struct OffsetAndSize
    {
        uint16_t offset, size;
        OffsetAndSize(uint16_t o, uint16_t s) : offset(o), size(s) {}
    };
    std::unique_ptr<float[]> data;
    uint32_t numVertices, stride;
    std::map<VertexAttributeType, OffsetAndSize> declaration;
};

struct IndexArray
{
    //std::unique_ptr<void[]> data;
    uint32_t numIndices;
    IndexType dataType;
    PrimitiveType primitiveType;
};

struct Geometry
{
    std::vector<VertexArrayPtr> vertexArrays;
    std::vector<IndexArrayPtr> indexArrays;
};

}

#endif // GEOMETRY_H
