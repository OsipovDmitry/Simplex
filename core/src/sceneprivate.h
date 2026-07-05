#ifndef CORE_SCENEPRIVATE_H
#define CORE_SCENEPRIVATE_H

#include <memory>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SceneData;

class ScenePrivate
{
public:
    ScenePrivate(Scene&, const std::string&);

    const std::string& name() const;

    std::shared_ptr<SceneData>& sceneData();
    std::shared_ptr<Background>& background();
    std::shared_ptr<SceneRootNode>& sceneRootNode();
    std::shared_ptr<ListenerNode>& listenerNode();

    uint32_t& shadowAtlasSize();

private:
    Scene& d_;

    std::string m_name;
    std::shared_ptr<SceneData> m_sceneData;
    std::shared_ptr<Background> m_background;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;

    uint32_t m_shadowAtlasSize = 0u;
};

} // namespace core
} // namespace simplex

#endif // CORE_SCENEPRIVATE_H
