#ifndef CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
#define CORE_IMAGEBASEDLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class ImageBasedLightHandler;
class SceneData;

class ImageBasedLightNodePrivate : public LightNodePrivate
{
public:
    ImageBasedLightNodePrivate(ImageBasedLightNode&, const std::string &name);
    ~ImageBasedLightNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    std::shared_ptr<MaterialMap>& BRDFLutMap();
    std::shared_ptr<MaterialMap>& diffuseMap();
    std::shared_ptr<MaterialMap>& specularMap();
    float& contribution();

    void addToSceneData(const std::shared_ptr<SceneData>&);
    void removeFromSceneData();
    void changeInSceneData();

    std::shared_ptr<ImageBasedLightHandler>& handler();

private:
    glm::mat4x4 doAreaMatrix() override;
    utils::BoundingBox doAreaBoundingBox() override;

    std::shared_ptr<MaterialMap> m_BRDFLutMap;
    std::shared_ptr<MaterialMap> m_diffuseMap;
    std::shared_ptr<MaterialMap> m_specularMap;
    float m_contribution;
    std::shared_ptr<ImageBasedLightHandler> m_handler;

};

}
}

#endif // CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
