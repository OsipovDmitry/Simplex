#ifndef CORE_FRUSTUMCULLINGVISITOR_H
#define CORE_FRUSTUMCULLINGVISITOR_H

#include <utils/forwarddecl.h>
#include <utils/pimpl.h>

#include <core/conditionalnodevisitor.h>


namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT FrustumCullingVisitor : public ConditionalNodeVisitor
{
public:
    FrustumCullingVisitor(const utils::Frustum&);
    ~FrustumCullingVisitor() override;

    const utils::Frustum &transformedFrustum() const; // here will be put transformed frustum of node's transform after call FrustumCullingVisitor::visit

};

}
}

#endif // CORE_FRUSTUMCULLINGVISITOR_H
