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
          SingleVec2,
          SingleVec3,
          SingleVec4,
          SingleMat2,
          SingleMat3,
          SingleMat4,

          Double,
          DoubleVec2,
          DoubleVec3,
          DoubleVec4,
          DoubleMat2,
          DoubleMat3,
          DoubleMat4,

          Int8,
          Int16,
          Int32,
          Int32Vec2,
          Int32Vec3,
          Int32Vec4,

          Uint8,
          Uint16,
          Uint32,
          Uint32Vec2,
          Uint32Vec3,
          Uint32Vec4,

          Sampler1D,
          Sampler2D,
          Sampler3D,
          SamplerCube,
          Sampler1DArray,
          Sampler2DArray,
          SamplerCubeArray,
          SamplerRect,
          Sampler1DShadow,
          Sampler2DShadow,
          SamplerCubeShadow,
          Sampler1DArrayShadow,
          Sampler2DArrayShadow,
          SamplerCubeArrayShadow)

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
