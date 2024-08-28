#ifndef CORE_STATESETPRIVATE_H
#define CORE_STATESETPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class StateSetPrivate
{
public:
    StateSetPrivate();
    virtual ~StateSetPrivate();

    UniformCollection &uniformCollection();
    UserUniformCollection &userUniformCollection();
    SSBOCollection &ssboCollection();

protected:
    UniformCollection m_uniformCollection;
    UserUniformCollection m_userUniformCollection;
    SSBOCollection m_SSBOCollection;
};

}
}

#endif // CORE_STATESETPRIVATE_H
