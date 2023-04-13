#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

#include <unordered_set>
#include <memory>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawableNodePrivate : public NodePrivate
{
public:
    DrawableNodePrivate(const std::string &name)
        : NodePrivate(name)
    {}

    const std::unordered_set<std::shared_ptr<Drawable>> &drawables() const { return m_drawables; }
    std::unordered_set<std::shared_ptr<Drawable>> &drawables() { return m_drawables; }

private:
    std::unordered_set<std::shared_ptr<Drawable>> m_drawables;
};

}
}

#endif // CORE_DRAWABLENODEPRIVATE_H
