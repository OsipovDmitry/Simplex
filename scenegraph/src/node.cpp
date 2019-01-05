#include <algorithm>

#include <logger/logger.h>

#include <scenegraph/node.h>
#include <scenegraph/scene.h>
#include <scenegraph/abstractsceneoptimizer.h>

#include "scene_p.h"
#include "node_p.h"

namespace scenegraph
{

ScenePtr Node::scene() const
{
    return m_->scene.lock();
}

NodePtr Node::parent() const
{
    return m_->parent.lock();
}

const NodeList Node::children() const
{
    return m_->children;
}

void Node::detach(NodePtr node)
{
    if (node->m_->parent.lock() != shared_from_this())
    {
        LOG_ERROR("Can not detach node");
        return;
    }

    auto iter = std::find(m_->children.begin(), m_->children.end(), node);
    if (iter != m_->children.end())
    {
        static_cast<ScenePrivate*>(m_->scene.lock()->m())->optimizer->nodeDetached(node.get());
        m_->children.erase(iter);
        node->m()->parent.reset();
    }
}

void Node::attach(NodePtr node)
{
    if (node->m_->scene.lock() != m_->scene.lock())
    {
        LOG_ERROR("Can not attach node");
        return;
    }

    NodePtr parent = node->parent();

    if (parent.get() == this)
        return;

    if (parent)
        parent->detach(node);

    m_->children.push_back(node);
    node->m_->parent = shared_from_this();
    static_cast<ScenePrivate*>(m_->scene.lock()->m())->optimizer->nodeAttached(node.get());
}

glm::vec3 Node::position() const
{
    return m_->position;
}

void Node::setPosition(const glm::vec3 &value)
{
    m_->position = value;
    m_->updateTransform();
}

glm::quat Node::orientation() const
{
    return m_->orientation;
}

void Node::setOrientation(const glm::quat &value) const
{
    m_->orientation = value;
    m_->updateTransform();
}

const glm::mat4x4 &Node::localTransform() const
{
    if (m_->needUpdateTransform)
        m_->recalcTransform();

    return m_->cacheLocalTransform;
}

const glm::mat4x4 &Node::worldTransform() const
{
    if (m_->needUpdateTransform)
        m_->recalcTransform();

    return m_->cacheWorldTransform;
}

NodePtr Node::create(ScenePtr scene)
{
    return NodePtr(new Node(scene), NodeDeleter());;
}

Node::Node(NodePrivate *p) :
    m_(p)
{
}

Node::Node(ScenePtr scene) :
    m_(new NodePrivate(this, scene))
{
    static_cast<ScenePrivate*>(m_->scene.lock()->m())->optimizer->nodeCreated(this);
}

Node::~Node()
{
    if (m_->scene.expired() == false)
    {
        auto optimizer = static_cast<ScenePrivate*>(m_->scene.lock()->m())->optimizer;

        for (auto child: m_->children)
        {
            optimizer->nodeDetached(child.get());
            child->m_->parent.reset();
        }

        optimizer->nodeDestroyed(this);
    }

    delete m_;
}

}
