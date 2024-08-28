#ifndef CORE_VISUALDRAWABLENODE_H
#define CORE_VISUALDRAWABLENODE_H

#include <unordered_set>

#include <utils/pimpl.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

class VisualDrawableNodePrivate;
class CORE_SHARED_EXPORT VisualDrawableNode : public Node
{
    PRIVATE_IMPL(VisualDrawableNode)
public:
    VisualDrawableNode(const std::string&);
    ~VisualDrawableNode() override;

    std::shared_ptr<VisualDrawableNode> asVisualDrawableNode() override;
    std::shared_ptr<const VisualDrawableNode> asVisualDrawableNode() const override;

    const std::unordered_set<std::shared_ptr<VisualDrawable>> &visualDrawables() const;
    void addVisualDrawable(std::shared_ptr<VisualDrawable>);
    void removeVisualDrawable(std::shared_ptr<VisualDrawable>);

};

}
}


#endif // CORE_VISUALDRAWABLENODE_H
