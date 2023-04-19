#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include <cstdint>

#include <utils/enumclass.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(Type, uint16_t,
          Undefined,

          Single,
          Double,

          Int8,
          Int16,
          Int32,

          Uint8,
          Uint16,
          Uint32)


template<typename T> inline Type toType() { return Type::Undefined; }
template<> inline Type toType<float>() { return Type::Single; }
template<> inline Type toType<double>() { return Type::Double; }
template<> inline Type toType<int8_t>() { return Type::Int8; }
template<> inline Type toType<int16_t>() { return Type::Int16; }
template<> inline Type toType<int32_t>() { return Type::Int32; }
template<> inline Type toType<uint8_t>() { return Type::Uint8; }
template<> inline Type toType<uint16_t>() { return Type::Uint16; }
template<> inline Type toType<uint32_t>() { return Type::Uint32; }


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
