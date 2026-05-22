#include "imagebasedlightnodeprivate.h"

#include <core/imagebasedlightnode.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

ImageBasedLightNodePrivate::ImageBasedLightNodePrivate(ImageBasedLightNode& iblLightNode, const std::string& name)
    : LightNodePrivate(iblLightNode, name, LightType::ImageBased)
    , m_contribution(0.f)
{
}

ImageBasedLightNodePrivate::~ImageBasedLightNodePrivate() = default;

uint32_t ImageBasedLightNodePrivate::shadowLayersCount() const
{
    return 0u;
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

std::shared_ptr<LightHandler> ImageBasedLightNodePrivate::createLightInSceneData(SceneData& sceneData) const
{
    return sceneData.addImageBasedLight(
        d().globalTransform(), m_isLightingEnabled, m_BRDFLutMap, m_diffuseMap, m_specularMap, m_contribution);
}

void ImageBasedLightNodePrivate::updateLightInSceneData(LightHandler& handler) const
{
    if (auto sceneData = handler.sceneData().lock())
        sceneData->onImageBasedLightChanged(
            handler, d().globalTransform(), m_isLightingEnabled, m_BRDFLutMap, m_diffuseMap, m_specularMap, m_contribution);
}

} // namespace core
} // namespace simplex
