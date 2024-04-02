#ifndef CORE_IBLLIGHTNODEPRIVATE_H
#define CORE_IBLLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class IBLLightNodePrivate : public LightNodePrivate
{
public:
    IBLLightNodePrivate(const std::string &name);
    ~IBLLightNodePrivate() override;

    std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&) const override;
    const glm::mat4x4 &shadowBiasMatrix() const override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();
    static std::weak_ptr<const graphics::ITexture> &defaultBRDFLutMap();
    static std::weak_ptr<const graphics::ITexture> &defaultDiffuseMap();
    static std::weak_ptr<const graphics::ITexture> &defaultSpecularMap();
    static float &defaultContribution();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;
    static std::weak_ptr<const graphics::ITexture> s_defaultBRDFLutMap;
    static std::weak_ptr<const graphics::ITexture> s_defaultDiffuseMap;
    static std::weak_ptr<const graphics::ITexture> s_defaultSpecularMap;
    static float s_defaultContribution;

};

}
}

#endif // CORE_IBLLIGHTNODEPRIVATE_H
