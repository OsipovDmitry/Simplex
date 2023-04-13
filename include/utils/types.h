#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include <cstdint>

#include <utils/enumclass.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(Type, uint16_t,
          Single,
          Double,

          Int8,
          Int16,
          Int32,

          Uint8,
          Uint16,
          Uint32)

ENUMCLASS(PrimitiveType, uint16_t,
          Points,
          Lines,
          LineStrip,
          Triangles,
          TriangleStrip,
          TriangleFan)

ENUMCLASS(VertexAttribute, uint16_t,
          Position,
          Normal,
          TexCoord,
          BonesIDs,
          BonesWeights,
          Tangent,
          Color)

}
}

#endif // UTILS_TYPES_H
