#ifndef TYPEINFO_H
#define TYPEINFO_H

#include <cstdint>

#include <utils/utilsglobal.h>
#include <utils/types.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT TypeInfo
{
public:
    explicit TypeInfo(Type);

    bool isFloat() const;
    bool isSignedInt() const;
    bool isUnsignedInt() const;
    bool isInt() const;
    uint32_t size() const;

    static bool isFloat(Type);
    static bool isSignedInt(Type);
    static bool isUnsignedInt(Type);
    static bool isInt(Type);
    static uint32_t size(Type);

private:
    Type m_type;
};

}
}

#endif // TYPEINFO_H
