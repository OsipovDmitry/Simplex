#ifndef CORE_STATICBODYNODE_H
#define CORE_STATICBODYNODE_H

#include <utils/pimpl.h>

#include <core/bodynode.h>

namespace simplex
{
namespace core
{

class StaticBodyNodePrivate;
class CORE_SHARED_EXPORT StaticBodyNode : public BodyNode
{
    PRIVATE_IMPL(StaticBodyNode)
public:
    StaticBodyNode(const std::string&);
    ~StaticBodyNode() override;

    std::shared_ptr<StaticBodyNode> asStaticBodyNode() override;
    std::shared_ptr<const StaticBodyNode> asStaticBodyNode() const override;

    std::shared_ptr<physics::IStaticBody> staticBody();
    std::shared_ptr<const physics::IStaticBody> staticBody() const;
};

} // namespace core
} // namespace simplex

#endif // CORE_STATICBODYNODE_H
