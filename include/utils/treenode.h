#ifndef UTILS_TREENODE_H
#define UTILS_TREENODE_H

#include <vector>
#include <memory>
#include <algorithm>

#include "noncopyble.h"

namespace simplex
{
namespace utils
{

template <typename T>
class TreeNode
{
    NONCOPYBLE(TreeNode<T>)
public:
    TreeNode() {}
    virtual ~TreeNode()
    {
        clear();
    }

    void clear()
    {
        for (auto &chld : m_children)
        {
            chld->TreeNode<T>::doDetach();
            chld->m_parent = nullptr;
        }
        m_children.clear();
    }

    void attach(std::shared_ptr<T> node)
    {
        if (canAttach(node))
        {
            if (node->m_parent)
                node->m_parent->detach(node);
            node->m_parent = dynamic_cast<T*>(this);
            m_children.push_back(node);
            node->TreeNode<T>::doAttach();
        }
    }

    bool detach(std::shared_ptr<T> node)
    {
        if (node->m_parent != this)
            return false;
        auto it = std::find(m_children.begin(), m_children.end(), node);
        if (it == m_children.end())
            return false;
        if (!canDetach(node))
            return false;

        node->TreeNode<T>::doDetach();
        m_children.erase(it);
        node->m_parent = nullptr;
        return true;
    }

    int32_t relationDegree(std::shared_ptr<const T> grandParent) const
    {
        int32_t degree = 0;
        auto thisNode = this;
        while (thisNode && thisNode != grandParent.get())
            thisNode = thisNode->m_parent;
        return thisNode ? degree : -1;
    }

    std::shared_ptr<const T> parent() const { return m_parent ? m_parent->shared_from_this() : nullptr; }
    std::shared_ptr<T> parent() { return m_parent ? m_parent->shared_from_this() : nullptr; }

    const std::vector<std::shared_ptr<T>> &children() const { return m_children; }
    std::vector<std::shared_ptr<T>> &children() { return m_children; }

protected:
    virtual bool canAttach(std::shared_ptr<T>) { return true; }
    virtual bool canDetach(std::shared_ptr<T>) { return true; }

    virtual void doAttach() {}
    virtual void doDetach() {}

    std::vector<std::shared_ptr<T>> m_children;
    T* m_parent = nullptr;
};

} // namespace
} // namespace

#endif // UTILS_TREENODE_H
