#ifndef CORE_VISUALDRAWABLENODEPRIVATE_H
#define CORE_VISUALDRAWABLENODEPRIVATE_H

#include <unordered_set>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class VisualDrawableNodePrivate : public NodePrivate
{
public:
    VisualDrawableNodePrivate(VisualDrawableNode&, const std::string&);
    ~VisualDrawableNodePrivate() override;

    utils::BoundingBox doBoundingBox() override;

    std::unordered_set<std::shared_ptr<VisualDrawable>> &visualDrawables();

    const utils::BoundingBox &localBoundingBox();

    void dirtyLocalBoundingBox();

    void update(const std::shared_ptr<graphics::RendererBase>&);

private:
    std::unordered_set<std::shared_ptr<VisualDrawable>> m_visualDrawables;
    utils::BoundingBox m_localBoundingBox;
    bool m_isLocalBoundingBoxDirty;
};

}
}

#endif // CORE_VISUALDRAWABLENODEPRIVATE_H
