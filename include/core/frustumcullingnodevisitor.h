#ifndef CORE_FRUSTUMCULLINGNODEVISITOR_H
#define CORE_FRUSTUMCULLINGNODEVISITOR_H

#include <deque>

#include <utils/forwarddecl.h>
#include <utils/frustum.h>
#include <utils/pimpl.h>

#include <core/conditionalnodevisitor.h>


namespace simplex
{
namespace core
{

class FrustumCullingNodeVisitorPrivate;
class CORE_SHARED_EXPORT FrustumCullingNodeVisitor : public ConditionalNodeVisitor
{
    PIMPL(FrustumCullingNodeVisitor)
public:
    class FrustumCullingCondition : public ConditionalNodeVisitor::AbstractCondition
    {
    public:
        FrustumCullingCondition(const utils::Frustum &frustum);

        bool operator ()(std::shared_ptr<Node> node) override;
        const utils::Frustum &transformedFrustum() const;

    private:
        utils::Frustum m_frustum;
        utils::Frustum m_transformedFrustum;
    };

    FrustumCullingNodeVisitor(const utils::Frustum&);
    ~FrustumCullingNodeVisitor() override;

    bool visit(std::shared_ptr<Node>) override;

    const std::deque<std::shared_ptr<Node>> &nodes() const;
    const utils::Frustum &transformedFrustum() const; // here will be put transformed frustum of node's transform after call FrustumCullingVisitor::visit

protected:
    FrustumCullingNodeVisitor(FrustumCullingNodeVisitorPrivate*);

};

}
}

#endif // CORE_FRUSTUMCULLINGNODEVISITOR_H
