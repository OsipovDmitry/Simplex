#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightHandler;
class SceneData;

class SpotLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(SpotLightNode)

public:
    SpotLightNodePrivate(SpotLightNode&, const std::string&);
    ~SpotLightNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    glm::vec3& color();
    glm::vec2& radiuses();
    glm::vec2& halfAngles();

    void addToSceneData(const std::shared_ptr<SceneData>&);
    void removeFromSceneData();
    void changeInSceneData();

    std::shared_ptr<SpotLightHandler>& handler();

private:
    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

    glm::vec3 doAreaScale() const;

    glm::vec3 m_color;
    glm::vec2 m_radiuses;
    glm::vec2 m_halfAngles;
    std::shared_ptr<SpotLightHandler> m_handler;
};

}
}

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
