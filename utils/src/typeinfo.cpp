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

bool TypeInfo::isFloat() const
{
    return isFloat(m_type);
}

bool TypeInfo::isSignedInt() const
{
    return isSignedInt(m_type);
}

bool TypeInfo::isUnsignedInt() const
{
    return isUnsignedInt(m_type);
}

bool TypeInfo::isInt() const
{
    return isInt(m_type);
}

uint32_t TypeInfo::size() const
{
    return size(m_type);
}

bool TypeInfo::isFloat(Type type)
{
    return (castFromType(type) >= castFromType(Type::Single)) && (castFromType(type) <= castFromType(Type::Double));
}

bool TypeInfo::isSignedInt(Type type)
{
    return (castFromType(type) >= castFromType(Type::Int8)) && (castFromType(type) <= castFromType(Type::Int32));
}

bool TypeInfo::isUnsignedInt(Type type)
{
    return (castFromType(type) >= castFromType(Type::Uint8)) && (castFromType(type) <= castFromType(Type::Uint32));
}

bool TypeInfo::isInt(Type type)
{
    return isSignedInt(type) || isUnsignedInt(type);
}

uint32_t TypeInfo::size(Type type)
{
    switch (type)
    {
    case Type::Single : return sizeof(float);
    case Type::Double : return sizeof(double);
    case Type::Int8 : return sizeof(int8_t);
    case Type::Int16 : return sizeof(int16_t);
    case Type::Int32 : return sizeof(int32_t);
    case Type::Uint8 : return sizeof(uint8_t);
    case Type::Uint16 : return sizeof(uint16_t);
    case Type::Uint32 : return sizeof(uint32_t);
    default: return 0;
    }
}

}
}
