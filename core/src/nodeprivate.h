#ifndef CORE_NODEPRIVATE_H
#define CORE_NODEPRIVATE_H

#include <string>

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
    virtual void doAttachToParent(const std::shared_ptr<Node>&);
    virtual void doAttachToScene(const std::shared_ptr<Scene>&);
    virtual void doDetachFromParent(const std::shared_ptr<Node>&);
    virtual void doDetachFromScene(const std::shared_ptr<Scene>&);

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
