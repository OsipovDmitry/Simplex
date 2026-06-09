#ifndef CORE_BODYNODE_H
#define CORE_BODYNODE_H

#include <utils/pimpl.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

class BodyNodePrivate;
class CORE_SHARED_EXPORT BodyNode : public Node
{
    PRIVATE_IMPL(BodyNode)
public:
    ~BodyNode() override;

    std::shared_ptr<BodyNode> asBodyNode() override;
    std::shared_ptr<const BodyNode> asBodyNode() const override;

    std::shared_ptr<physics::IBody> body();
    std::shared_ptr<const physics::IBody> body() const;

    virtual std::shared_ptr<StaticBodyNode> asStaticBodyNode();
    virtual std::shared_ptr<const StaticBodyNode> asStaticBodyNode() const;

    virtual std::shared_ptr<KinematicBodyNode> asKinematicBodyNode();
    virtual std::shared_ptr<const KinematicBodyNode> asKinematicBodyNode() const;

    virtual std::shared_ptr<DynamicBodyNode> asDynamicBodyNode();
    virtual std::shared_ptr<const DynamicBodyNode> asDynamicBodyNode() const;

protected:
    BodyNode(std::unique_ptr<BodyNodePrivate>);
};

} // namespace core
} // namespace simplex

#endif // CORE_BODYNODE_H
