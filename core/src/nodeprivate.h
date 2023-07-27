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
    NodePrivate(const std::string &name);
    virtual ~NodePrivate();

    const std::string &name() const;

    bool &isGlobalTransformDirty();
    bool &isBoundingBoxDirty();

    utils::Transform &transform();
    utils::Transform &globalTransform();

    utils::BoundingBox &globalBoundingBox();

    virtual void doUpdate(uint64_t, uint32_t);

    static void dirtyGlobalTransform(std::shared_ptr<Node>);
    static void dirtyBoundingBox(std::shared_ptr<Node>);

protected:
    std::string m_name;

    utils::Transform m_transform;
    utils::Transform m_globalTransform;

    utils::BoundingBox m_boundingBox;

    bool m_globalTransfomDirty;
    bool m_boundingBoxDirty;

};

}
}

#endif // CORE_NODEPRIVATE_H
