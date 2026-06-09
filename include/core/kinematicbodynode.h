#ifndef CORE_KINEMATICBODYNODE_H
#define CORE_KINEMATICBODYNODE_H

#include <utils/pimpl.h>

#include <core/bodynode.h>

namespace simplex
{
namespace core
{

class KinematicBodyNodePrivate;
class CORE_SHARED_EXPORT KinematicBodyNode : public BodyNode
{
    PRIVATE_IMPL(KinematicBodyNode)
public:
    KinematicBodyNode(const std::string&);
    ~KinematicBodyNode() override;

    std::shared_ptr<KinematicBodyNode> asKinematicBodyNode() override;
    std::shared_ptr<const KinematicBodyNode> asKinematicBodyNode() const override;

    std::shared_ptr<physics::IKinematicBody> kinematicBody();
    std::shared_ptr<const physics::IKinematicBody> kinematicBody() const;
};

} // namespace core
} // namespace simplex

#endif // CORE_KYNEMATICBODYNODE_H
