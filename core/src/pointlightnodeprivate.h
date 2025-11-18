#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class LightHandler;
class SceneData;

class PointLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(PointLightNode)

public:
    PointLightNodePrivate(PointLightNode&, const std::string&);
    ~PointLightNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    glm::vec3& color();
    glm::vec2& radiuses();

    void addToSceneData(const std::shared_ptr<SceneData>&);
    void removeFromSceneData();
    void changeInSceneData();

    std::shared_ptr<LightHandler>& handler();

private:
    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::vec3 m_color;
    glm::vec2 m_radiuses;
    std::shared_ptr<LightHandler> m_handler;

};

}
}

#endif // CORE_POINTLIGHTNODEPRIVATE_H
