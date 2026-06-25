#ifndef CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
#define CORE_IMAGEBASEDLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class ImageBasedLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(ImageBasedLightNode)
public:
    ImageBasedLightNodePrivate(ImageBasedLightNode&, const std::string& name);
    ~ImageBasedLightNodePrivate() override;

    std::shared_ptr<MaterialMap>& BRDFLutMap();
    std::shared_ptr<MaterialMap>& diffuseMap();
    std::shared_ptr<MaterialMap>& specularMap();
    float& contribution();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    std::shared_ptr<MaterialMap> m_BRDFLutMap;
    std::shared_ptr<MaterialMap> m_diffuseMap;
    std::shared_ptr<MaterialMap> m_specularMap;
    float m_contribution;
};

} // namespace core
} // namespace simplex

#endif // CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
