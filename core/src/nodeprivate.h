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
    bool &isGlobalBoundingBoxDirty();

    virtual utils::Transform &localTransform();
    virtual utils::Transform &globalTransform();

    virtual utils::BoundingBox &minimalBoundingBox();
    virtual utils::BoundingBox &localBoundingBox();
    virtual utils::BoundingBox &globalBoundingBox();

    virtual void doUpdate(uint64_t, uint32_t);

    static void dirtyGlobalTransform(std::shared_ptr<Node>);
    static void dirtyGlobalBoundingBox(std::shared_ptr<Node>);

private:
    std::string m_name;

    utils::Transform m_localTransform;
    utils::Transform m_globalTransform;

    utils::BoundingBox m_minimalBoundingBox;
    utils::BoundingBox m_globalBoundingBox;

    bool m_globalTransfomDirty;
    bool m_globalBoundingBoxDirty;

};

}
}

#endif // CORE_NODEPRIVATE_H
