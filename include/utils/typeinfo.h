#ifndef UTILS_TYPEINFO_H
#define UTILS_TYPEINFO_H

#include <cstdint>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT TypeInfo
{
public:
    explicit TypeInfo(Type);

    Type type() const;

    bool isDefined() const;
    bool isFloat() const;
    bool isSignedInt() const;
    bool isUnsignedInt() const;
    bool isInt() const;
    uint32_t size() const;

    static bool isDefined(Type);
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

#endif // UTILS_TYPEINFO_H
