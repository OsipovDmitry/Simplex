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
    UniformBlockCollection& uniformBlockCollection();
    ShaderStorageBlockCollection& shaderStorageBlockCollection();

protected:
    UniformCollection m_uniformCollection;
    UserUniformCollection m_userUniformCollection;
    UniformBlockCollection m_uniformBlockCollection;
    ShaderStorageBlockCollection m_shaderStorageBlockCollection;
};

}
}

#endif // CORE_STATESETPRIVATE_H
