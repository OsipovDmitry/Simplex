#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class LightNodePrivate : public NodePrivate
{
public:
    LightNodePrivate(const std::string &name);
    ~LightNodePrivate() override;

    glm::vec3 &color();

private:
    glm::vec3 m_color;

};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
