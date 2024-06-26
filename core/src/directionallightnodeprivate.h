#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate : public LightNodePrivate
{
public:
    DirectionalLightNodePrivate(const std::string &name);
    ~DirectionalLightNodePrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&) const override;
    const glm::mat4x4 &shadowBiasMatrix() const override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;

};

}
}

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
