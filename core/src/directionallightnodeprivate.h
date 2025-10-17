#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightHandler;
class SceneData;

class DirectionalLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(DirectionalLightNode)

public:
    DirectionalLightNodePrivate(DirectionalLightNode&, const std::string&);
    ~DirectionalLightNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    glm::vec3& color();

    void addToSceneData(const std::shared_ptr<SceneData>&);
    void removeFromSceneData();
    void changeInSceneData();

    std::shared_ptr<DirectionalLightHandler>& handler();

private:
    ShadowTransform doShadowTransform(const utils::Frustum::Points&) override;

    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

    glm::vec3 m_color;
    std::shared_ptr<DirectionalLightHandler> m_handler;
};

}
}

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
