#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat2x2.hpp>
#include <utils/glm/mat3x3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <utils/typeinfo.h>
#include <utils/types.h>

namespace simplex
{
namespace utils
{

TypeInfo::TypeInfo(Type type)
    : m_type(type)
{
}

Type TypeInfo::type() const
{
    return m_type;
}

bool TypeInfo::isDefined(Type type)
{
    return type != Type::Undefined;
}

bool TypeInfo::isSingleScalar(Type type)
{
    return (castFromType(type) >= castFromType(Type::Single)) && (castFromType(type) <= castFromType(Type::Single));
}

bool TypeInfo::isSingleVec(Type type)
{
    return (castFromType(type) >= castFromType(Type::SingleVec2)) && (castFromType(type) <= castFromType(Type::SingleVec4));
}

bool TypeInfo::isSingleMat(Type type)
{
    return (castFromType(type) >= castFromType(Type::SingleMat2)) && (castFromType(type) <= castFromType(Type::SingleMat4));
}

bool TypeInfo::isDoubleScalar(Type type)
{
    return (castFromType(type) >= castFromType(Type::Double)) && (castFromType(type) <= castFromType(Type::Double));
}

bool TypeInfo::isDoubleVec(Type type)
{
    return (castFromType(type) >= castFromType(Type::DoubleVec2)) && (castFromType(type) <= castFromType(Type::DoubleVec4));
}

bool TypeInfo::isDoubleMat(Type type)
{
    return (castFromType(type) >= castFromType(Type::DoubleMat2)) && (castFromType(type) <= castFromType(Type::DoubleMat2));
}

bool TypeInfo::isSignedIntScalar(Type type)
{
    return (castFromType(type) >= castFromType(Type::Int8)) && (castFromType(type) <= castFromType(Type::Int32));
}

bool TypeInfo::isSignedIntVec(Type type)
{
    return (castFromType(type) >= castFromType(Type::Int32Vec2)) && (castFromType(type) <= castFromType(Type::Int32Vec4));
}

bool TypeInfo::isUnsignedIntScalar(Type type)
{
    return (castFromType(type) >= castFromType(Type::Uint8)) && (castFromType(type) <= castFromType(Type::Uint32));
}

bool TypeInfo::isUnsignedIntVec(Type type)
{
    return (castFromType(type) >= castFromType(Type::Uint32Vec2)) && (castFromType(type) <= castFromType(Type::Uint32Vec4));
}

bool TypeInfo::isFloatScalar(Type type)
{
    return isSingleScalar(type) || isDoubleScalar(type);
}

bool TypeInfo::isFloatVec(Type type)
{
    return isSingleVec(type) || isDoubleVec(type);
}

bool TypeInfo::isFloatMat(Type type)
{
    return isFloatMat(type) || isDoubleMat(type);
}

bool TypeInfo::isIntScalar(Type type)
{
    return isSignedIntScalar(type) || isUnsignedIntScalar(type);
}

bool TypeInfo::isIntVec(Type type)
{
    return isSignedIntVec(type) || isUnsignedIntVec(type);
}

bool TypeInfo::isScalar(Type type)
{
    return isFloatScalar(type) || isIntScalar(type);
}

bool TypeInfo::isVec(Type type)
{
    return isFloatVec(type) || isIntVec(type);
}

bool TypeInfo::isMat(Type type)
{
    return isFloatMat(type);
}

bool TypeInfo::isSampler(Type type)
{
    return (castFromType(type) >= castFromType(Type::Sampler1D)) && (castFromType(type) <= castFromType(Type::SamplerCubeArrayShadow));
}

uint32_t TypeInfo::size(Type type)
{
    switch (type)
    {
    case Type::Single : return sizeof(float);
    case Type::SingleVec2 : return sizeof(glm::vec2);
    case Type::SingleVec3 : return sizeof(glm::vec3);
    case Type::SingleVec4 : return sizeof(glm::vec4);
    case Type::SingleMat2 : return sizeof(glm::mat2);
    case Type::SingleMat3 : return sizeof(glm::mat3);
    case Type::SingleMat4 : return sizeof(glm::mat4);
    case Type::Double : return sizeof(double);
    case Type::DoubleVec2 : return sizeof(glm::dvec2);
    case Type::DoubleVec3 : return sizeof(glm::dvec3);
    case Type::DoubleVec4 : return sizeof(glm::dvec4);
    case Type::DoubleMat2 : return sizeof(glm::dmat2);
    case Type::DoubleMat3 : return sizeof(glm::dmat3);
    case Type::DoubleMat4 : return sizeof(glm::dmat4);
    case Type::Int8 : return sizeof(int8_t);
    case Type::Int16 : return sizeof(int16_t);
    case Type::Int32 : return sizeof(int32_t);
    case Type::Int32Vec2 : return sizeof(glm::i32vec2);
    case Type::Int32Vec3 : return sizeof(glm::i32vec3);
    case Type::Int32Vec4 : return sizeof(glm::i32vec4);
    case Type::Uint8 : return sizeof(uint8_t);
    case Type::Uint16 : return sizeof(uint16_t);
    case Type::Uint32 : return sizeof(uint32_t);
    case Type::Uint32Vec2 : return sizeof(glm::u32vec2);
    case Type::Uint32Vec3 : return sizeof(glm::u32vec3);
    case Type::Uint32Vec4 : return sizeof(glm::u32vec4);
    case Type::Sampler1D :
    case Type::Sampler2D :
    case Type::Sampler3D :
    case Type::SamplerCube :
    case Type::Sampler1DArray :
    case Type::Sampler2DArray :
    case Type::SamplerCubeArray :
    case Type::SamplerRect :
    case Type::Sampler1DShadow :
    case Type::Sampler2DShadow :
    case Type::SamplerCubeShadow :
    case Type::Sampler1DArrayShadow :
    case Type::Sampler2DArrayShadow :
    case Type::SamplerCubeArrayShadow : return sizeof(uint32_t);
    default: return 0;
    }
}

Type TypeInfo::makeVecType(Type scalarType, uint32_t numComponents)
{
    auto result = Type::Undefined;

    switch (scalarType)
    {
    case Type::Single: {
        switch (numComponents)
        {
        case 1: { result = Type::Single; break; }
        case 2: { result = Type::SingleVec2; break; }
        case 3: { result = Type::SingleVec3; break; }
        case 4: { result = Type::SingleVec4; break; }
        default: { break; }
        }
        break;
    }
    case Type::Double: {
        switch (numComponents)
        {
        case 1: { result = Type::Double; break; }
        case 2: { result = Type::DoubleVec2; break; }
        case 3: { result = Type::DoubleVec3; break; }
        case 4: { result = Type::DoubleVec4; break; }
        default: { break; }
        }
        break;
    }
    case Type::Int32: {
        switch (numComponents)
        {
        case 1: { result = Type::Int32; break; }
        case 2: { result = Type::Int32Vec2; break; }
        case 3: { result = Type::Int32Vec3; break; }
        case 4: { result = Type::Int32Vec4; break; }
        default: { break; }
        }
        break;
    }
    case Type::Uint32: {
        switch (numComponents)
        {
        case 1: { result = Type::Uint32; break; }
        case 2: { result = Type::Uint32Vec2; break; }
        case 3: { result = Type::Uint32Vec3; break; }
        case 4: { result = Type::Uint32Vec4; break; }
        default: { break; }
        }
        break;
    }
    default: {
        break;
    }
    }

    return result;
}

}
}
