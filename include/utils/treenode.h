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
class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
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
        for (auto &child : m_children)
        {
            std::dynamic_pointer_cast<TreeNode<T>>(child)->doDetach();
            child->m_parent.reset();
        }
        m_children.clear();
    }

    bool attach(const std::shared_ptr<T> &node)
    {
        if (auto p = node->m_parent.lock())
            p->detach(node);
        node->m_parent = std::dynamic_pointer_cast<T>(shared_from_this());
        m_children.push_back(node);
        std::dynamic_pointer_cast<TreeNode<T>>(node)->doAttach();
        return true;
    }

    bool detach(const std::shared_ptr<T> &node)
    {
        if (node->m_parent.lock() != shared_from_this())
            return false;
        auto it = std::find(m_children.begin(), m_children.end(), node);
        if (it == m_children.end())
            return false;

        std::dynamic_pointer_cast<TreeNode<T>>(node)->doDetach();
        m_children.erase(it);
        node->m_parent.reset();
        return true;
    }

    //int32_t relationDegree(const std::shared_ptr<T> &grandParent) const
    //{
    //    int32_t degree = 0;
    //    auto thisNode = this;
    //    while (thisNode && thisNode != grandParent.get())
    //        thisNode = thisNode->m_parent;
    //    return thisNode ? degree : -1;
    //}

    std::shared_ptr<const T> parent() const { return m_parent.expired() ? nullptr : m_parent.lock(); }
    std::shared_ptr<T> parent() { return m_parent.expired() ? nullptr : m_parent.lock(); }

    const std::vector<std::shared_ptr<T>> &children() const { return m_children; }
    std::vector<std::shared_ptr<T>> &children() { return m_children; }

protected:
    virtual void doAttach() {}
    virtual void doDetach() {}

    std::vector<std::shared_ptr<T>> m_children;
    std::weak_ptr<T> m_parent;
};

} // namespace
} // namespace

#endif // UTILS_TREENODE_H
