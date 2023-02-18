#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

#include <utils/utilsglobal.h>
#include <utils/enumclass.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(Type, uint32_t,
          Single,
          Double,

          Int8,
          Int16,
          Int32,

          Uint8,
          Uint16,
          Uint32)

class UTILS_SHARED_EXPORT TypeInfo
{
public:
    explicit TypeInfo(Type);

    bool isFloat();
    bool isSignedInt();
    bool isUnsignedInt();
    bool isInt();

    static bool isFloat(Type);
    static bool isSignedInt(Type);
    static bool isUnsignedInt(Type);
    static bool isInt(Type);

private:
    Type m_type;
};

}
}

#endif // TYPES_H
