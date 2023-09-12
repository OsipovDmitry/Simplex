#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

LightNodePrivate::LightNodePrivate(const std::string &name)
    : NodePrivate(name)
{
}

LightNodePrivate::~LightNodePrivate() = default;

glm::vec3 &LightNodePrivate::color()
{
    return m_color;
}

}
}
