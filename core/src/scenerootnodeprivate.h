#ifndef SCENEROOTNODEPRIVATE_H
#define SCENEROOTNODEPRIVATE_H

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class SceneRootNodePrivate : public NodePrivate
{
public:
    SceneRootNodePrivate(const std::string &name)
        : NodePrivate(name)
    {}

    std::weak_ptr<Scene> &scene() { return m_scene; }

private:
    std::weak_ptr<Scene> m_scene;

};

}
}

#endif // SCENEROOTNODEPRIVATE_H
