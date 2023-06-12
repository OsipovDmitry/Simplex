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

    bool isDefined() const { return isDefined(m_type); }

    bool isSingleScalar() const { return isSingleScalar(m_type); }
    bool isSingleVec() const { return isSingleVec(m_type); }
    bool isSingleMat() const { return isSingleMat(m_type); }

    bool isDoubleScalar() const { return isDoubleScalar(m_type); }
    bool isDoubleVec() const { return isDoubleVec(m_type); }
    bool isDoubleMat() const { return isDoubleMat(m_type); }

    bool isSignedIntScalar() const { return isSignedIntScalar(m_type); }
    bool isSignedIntVec() const { return isSignedIntVec(m_type); }

    bool isUnsignedIntScalar() const { return isUnsignedIntScalar(m_type); }
    bool isUnsignedIntVec() const { return isUnsignedIntVec(m_type); }

    bool isFloatScalar() const { return isFloatScalar(m_type); }
    bool isFloatVec() const { return isFloatVec(m_type); }
    bool isFloatMat() const { return isFloatMat(m_type); }

    bool isIntScalar() const { return isIntScalar(m_type); }
    bool isIntVec() const { return isIntVec(m_type); }

    bool isScalar() const { return isScalar(m_type); }
    bool isVec() const { return isVec(m_type); }
    bool isMat() const { return isMat(m_type); }

    bool isSampler() const { return isSampler(m_type); }

    uint32_t size();

    static bool isDefined(Type);

    static bool isSingleScalar(Type);
    static bool isSingleVec(Type);
    static bool isSingleMat(Type);

    static bool isDoubleScalar(Type);
    static bool isDoubleVec(Type);
    static bool isDoubleMat(Type);

    static bool isSignedIntScalar(Type);
    static bool isSignedIntVec(Type);

    static bool isUnsignedIntScalar(Type);
    static bool isUnsignedIntVec(Type);

    static bool isFloatScalar(Type);
    static bool isFloatVec(Type);
    static bool isFloatMat(Type);

    static bool isIntScalar(Type);
    static bool isIntVec(Type);

    static bool isScalar(Type);
    static bool isVec(Type);
    static bool isMat(Type);

    static bool isSampler(Type);

    static uint32_t size(Type);

    static Type makeVecType(Type scalarType, uint32_t numComponents);



private:
    Type m_type;
};

}
}

#endif // UTILS_TYPEINFO_H
