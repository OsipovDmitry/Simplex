#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawableNodePrivate : public NodePrivate
{
public:
    DrawableNodePrivate(const std::string &name);
    ~DrawableNodePrivate() override;

    bool &isLocalBoundingBoxDirty();
    utils::BoundingBox &localBoundingBox();

    std::unordered_set<std::shared_ptr<IDrawable>> &drawables();

private:
    std::unordered_set<std::shared_ptr<IDrawable>> m_drawables;
    utils::BoundingBox m_localBoundingBox;
    bool m_localBoundingBoxDirty;
};

}
}

#endif // CORE_DRAWABLENODEPRIVATE_H
