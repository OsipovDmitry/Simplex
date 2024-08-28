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

SSBOCollection &StateSetPrivate::ssboCollection()
{
    return m_SSBOCollection;
}

}
}
