#include <utils/types.h>

namespace simplex
{
namespace utils
{


TypeInfo::TypeInfo(Type type)
    : m_type(type)
{
}

bool TypeInfo::isFloat()
{
    return isFloat(m_type);
}

bool TypeInfo::isSignedInt()
{
    return isSignedInt(m_type);
}

bool TypeInfo::isUnsignedInt()
{
    return isUnsignedInt(m_type);
}

bool TypeInfo::isInt()
{
    return isInt(m_type);
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

}
}
