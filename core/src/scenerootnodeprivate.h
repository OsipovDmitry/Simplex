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
    SceneRootNodePrivate(const std::string &name, Scene *scene)
        : NodePrivate(name)
        , m_scene(scene)
    {}

    Scene *scene() { return m_scene; }

private:
    Scene *m_scene;
};

}
}

#endif // SCENEROOTNODEPRIVATE_H
