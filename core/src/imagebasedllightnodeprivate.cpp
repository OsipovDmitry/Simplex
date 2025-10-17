#include <core/imagebasedllightnode.h>
#include <core/scene.h>

#include "imagebasedllightnodeprivate.h"
#include "sceneprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

ImageBasedLightNodePrivate::ImageBasedLightNodePrivate(ImageBasedLightNode &iblLightNode, const std::string &name)
    : LightNodePrivate(iblLightNode, name, LightType::ImageBased)
    , m_contribution(0.f)
{
}

ImageBasedLightNodePrivate::~ImageBasedLightNodePrivate() = default;

void ImageBasedLightNodePrivate::doAfterTransformChanged()
{
    dirtyAreaMatrix();
    dirtyAreaBoundingBox();

    changeInSceneData();
}

void ImageBasedLightNodePrivate::doAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (auto& sceneData = scene->m().sceneData())
        addToSceneData(sceneData);
}

void ImageBasedLightNodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
{
    removeFromSceneData();
}

std::shared_ptr<MaterialMap>& ImageBasedLightNodePrivate::BRDFLutMap()
{
    return m_BRDFLutMap;
}

std::shared_ptr<MaterialMap>& ImageBasedLightNodePrivate::diffuseMap()
{
    return m_diffuseMap;
}

std::shared_ptr<MaterialMap>& ImageBasedLightNodePrivate::specularMap()
{
    return m_specularMap;
}

float& ImageBasedLightNodePrivate::contribution()
{
    return m_contribution;
}

void ImageBasedLightNodePrivate::addToSceneData(const std::shared_ptr<SceneData>& sceneData)
{
    if (!sceneData)
    {
        LOG_ERROR << "Scene data can't be nullptr";
        return;
    }

    m_handler = sceneData->addImageBasedLight(m_BRDFLutMap, m_diffuseMap, m_specularMap, m_contribution);
}

void ImageBasedLightNodePrivate::removeFromSceneData()
{
    m_handler.reset();
}

void ImageBasedLightNodePrivate::changeInSceneData()
{
    if (m_handler)
        if (auto sceneData = m_handler->sceneData().lock())
            sceneData->onImageBasedLightChanged(m_handler->ID(), m_BRDFLutMap, m_diffuseMap, m_specularMap, m_contribution);
}

std::shared_ptr<ImageBasedLightHandler>& ImageBasedLightNodePrivate::handler()
{
    return m_handler;
}

glm::mat4x4 ImageBasedLightNodePrivate::doAreaMatrix()
{
    return glm::mat4x4(1.f); // it is not used because bb policy is Root
}

utils::BoundingBox ImageBasedLightNodePrivate::doAreaBoundingBox()
{
    return utils::BoundingBox::empty(); // it is not used because bb policy is Root
}

}
}
