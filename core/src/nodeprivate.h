#ifndef CORE_NODEPRIVATE_H
#define CORE_NODEPRIVATE_H

#include <string>

#include <utils/boundingbox.h>
#include <utils/transform.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class NodePrivate
{
public:
    NodePrivate(Node&, const std::string& name);
    virtual ~NodePrivate();

    const Node& node() const;

    const std::string& name() const;

    utils::Transform& transform();
    utils::Transform& globalTransform();

    utils::BoundingBox& boundingBox();

    virtual const utils::BoundingBox& calculateBoundingBox();

    virtual void onUpdate(uint64_t, uint32_t);
    virtual void onBeforeTransformChanged();
    virtual void onAfterTransformChanged();
    virtual void onBoundingBoxChanged();
    virtual void onAttachToParent(const std::shared_ptr<Node>&);
    virtual void onAttachToScene(const std::shared_ptr<Scene>&);
    virtual void onDetachFromParent(const std::shared_ptr<Node>&);
    virtual void onDetachFromScene(const std::shared_ptr<Scene>&);

    bool& isGlobalTransfomDirty();
    void dirtyGlobalTransform();

    bool& isBoundingBoxDirty();
    void dirtyBoundingBox();

protected:
    Node& d_;

    std::string m_name;

    utils::Transform m_transform;
    utils::Transform m_globalTransform;

    utils::BoundingBox m_boundingBox;

    bool m_isGlobalTransfomDirty = true;
    bool m_isBoundingBoxDirty = true;
};

} // namespace core
} // namespace simplex

#endif // CORE_NODEPRIVATE_H
