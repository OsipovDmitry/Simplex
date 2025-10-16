#include "statesetprivate.h"

namespace simplex
{
namespace core
{

StateSetPrivate::StateSetPrivate()
{
}

StateSetPrivate::~StateSetPrivate() = default;

UniformCollection &StateSetPrivate::uniformCollection()
{
    return m_uniformCollection;
}

UserUniformCollection &StateSetPrivate::userUniformCollection()
{
    return m_userUniformCollection;
}

UniformBlockCollection& StateSetPrivate::uniformBlockCollection()
{
    return m_uniformBlockCollection;
}

ShaderStorageBlockCollection &StateSetPrivate::shaderStorageBlockCollection()
{
    return m_shaderStorageBlockCollection;
}

}
}
