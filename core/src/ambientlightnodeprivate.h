#ifndef CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
#define CORE_IMAGEBASEDLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class AmbientLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(AmbientLightNode)
public:
    AmbientLightNodePrivate(AmbientLightNode&, const std::string& name);
    ~AmbientLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    glm::vec3& color();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(SceneData&, utils::IDsGeneratorT<size_t>::value_type) const override;

    glm::vec3 m_color;
};

} // namespace core
} // namespace simplex

#endif // CORE_IMAGEBASEDLIGHTNODEPRIVATE_H
