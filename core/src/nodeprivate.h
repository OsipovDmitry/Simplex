#ifndef CORE_NODEPRIVATE_H
#define CORE_NODEPRIVATE_H

#include <string>
#include <memory>

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class NodePrivate
{
public:
    NodePrivate(Node&, const std::string &name);
    virtual ~NodePrivate();

    const Node &node() const;

    const std::string &name() const;

    utils::Transform &transform();
    utils::Transform &globalTransform();

    utils::BoundingBox &boundingBox();

    virtual void doUpdate(uint64_t, uint32_t);
    virtual utils::BoundingBox doBoundingBox();
    virtual void doBeforeTransformChanged();
    virtual void doAfterTransformChanged();
    virtual void doAttachToParent();
    virtual void doAttachToScene();
    virtual void doDetachFromParent();
    virtual void doDetachFromScene();

    bool &isGlobalTransfomDirty();
    bool &isBoundingBoxDirty();

    void dirtyGlobalTransform();
    void dirtyBoundingBox();

protected:
    Node &d_;

    std::string m_name;

    utils::Transform m_transform;
    utils::Transform m_globalTransform;

    utils::BoundingBox m_boundingBox;

    bool m_isGlobalTransfomDirty;
    bool m_isBoundingBoxDirty;
};

}
}

#endif // CORE_NODEPRIVATE_H
