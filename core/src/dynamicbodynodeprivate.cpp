#include "dynamicbodynodeprivate.h"

#include <core/dynamicbodynode.h>
#include <core/physicsrendererbase.h>

#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

DynamicBodyNodePrivate::DynamicBodyNodePrivate(DynamicBodyNode& dynamicBodyNode, const std::string& name)
    : BodyNodePrivate(dynamicBodyNode, name)
{
}

DynamicBodyNodePrivate::~DynamicBodyNodePrivate() = default;

std::shared_ptr<physics::IBody> DynamicBodyNodePrivate::body()
{
    return m_dynamicBody;
}

std::shared_ptr<physics::IDynamicBody>& DynamicBodyNodePrivate::dynamicBody()
{
    return m_dynamicBody;
}

void DynamicBodyNodePrivate::resetBodyTransformChangedAfterSimulation()
{
    if (const auto [isChanged, worldTransform] = m_dynamicBody->resetTransformChangedAfterSimulation(); isChanged)
    {
        for (auto& child : d_.children())
        {
            BeforeTransformChangedNodeVisitor beforeTransformChangedNodeVisitor;
            child->acceptDown(beforeTransformChangedNodeVisitor);
        }

        auto parentGlobalTransformInverted = utils::Transform::makeIdentity();
        if (auto p = d_.parent()) parentGlobalTransformInverted = p->globalTransform().inverted();

        m_transform = parentGlobalTransformInverted * worldTransform;
        dirtyGlobalTransform();

        for (auto& child : d_.children())
        {
            AfterTransformChangedNodeVisitor afterTransformChangedNodeVisitor;
            child->acceptDown(afterTransformChangedNodeVisitor);
        }

        if (auto p = d_.parent())
        {
            p->m().dirtyBoundingBox();

            BoundingBoxChangedNodeVisitor boundingBoxChangedNodeVisitor;
            p->acceptUp(boundingBoxChangedNodeVisitor);
        }
    }
}

} // namespace core
} // namespace simplex
