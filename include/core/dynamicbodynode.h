#ifndef CORE_DYNAMICBODYNODE_H
#define CORE_DYNAMICBODYNODE_H

#include <utils/pimpl.h>

#include <core/bodynode.h>

namespace simplex
{
namespace core
{

class DynamicBodyNodePrivate;
class CORE_SHARED_EXPORT DynamicBodyNode : public BodyNode
{
    PRIVATE_IMPL(DynamicBodyNode)
public:
    DynamicBodyNode(const std::string&);
    ~DynamicBodyNode() override;

    std::shared_ptr<DynamicBodyNode> asDynamicBodyNode() override;
    std::shared_ptr<const DynamicBodyNode> asDynamicBodyNode() const override;

    std::shared_ptr<physics::IDynamicBody> dynamicBody();
    std::shared_ptr<const physics::IDynamicBody> dynamicBody() const;
};

} // namespace core
} // namespace simplex

#endif // CORE_DYNAMICBODYNODE_H
