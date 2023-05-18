#ifndef CORE_ZNEARFARNODEVISITOR_H
#define CORE_ZNEARFARNODEVISITOR_H

#include <utils/forwarddecl.h>

#include <core/conditionalnodevisitor.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT ZNearFarNodeVisitor : public ConditionalNodeVisitor
{
public:
    ZNearFarNodeVisitor(const utils::OpenFrustum&); // zNear of input frustum must be 0.0
    ~ZNearFarNodeVisitor() override;

    const std::array<float, 2> &zNearFar() const;
};

} // namespace
} // namespace

#endif // CORE_ZNEARFARNODEVISITOR_H
