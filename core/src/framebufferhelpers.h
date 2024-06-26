#ifndef FRAMEBUFFERHELPERS_H
#define FRAMEBUFFERHELPERS_H

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class FrameBufferWrapper
{
public:
    FrameBufferWrapper(const std::shared_ptr<graphics::IFrameBuffer>&);
    virtual ~FrameBufferWrapper();

    const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer() const;

protected:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
};

class GFrameBuffer : public FrameBufferWrapper
{
public:
    GFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&);
    graphics::PConstTexture color0Texture() const;
    graphics::PConstTexture color1Texture() const;
    graphics::PConstTexture color2Texture() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture stencilTexture() const;
};

class OITFrameBuffer : public FrameBufferWrapper
{
public:
    OITFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    const graphics::PConstImage &oitDepthImage();
    const graphics::PConstImage &oitIndicesImage();

private:
    graphics::PConstImage m_oitDepthImage;
    graphics::PConstImage m_oitIndicesImage;
};

class LightFrameBuffer : public FrameBufferWrapper
{
public:
    LightFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void setForClearPass();
    void setForStencilPass();
    void setForLightPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    graphics::PConstTexture colorTexture() const;
};

class FinalFrameBuffer : public FrameBufferWrapper
{
public:
    FinalFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void setForBackgroundPass();
    void setForForegroundPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    graphics::PConstTexture colorTexture() const;
};

class PostprocessFrameBuffer : public FrameBufferWrapper
{
public:
    PostprocessFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&);
};

class ShadowFrameBuffer : public FrameBufferWrapper
{
public:
    ShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    uint32_t numLayers() const;

    void setForOpaquePass();
    void setForTransparentPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, bool);
    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;

protected:
    virtual graphics::PTexture doColorTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const = 0;
    virtual graphics::PTexture doDepthTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const = 0;
};

class ShadowFrameBuffer2D : public ShadowFrameBuffer
{
public:
    ShadowFrameBuffer2D(const std::shared_ptr<graphics::IRenderer>&);

protected:
    graphics::PTexture doColorTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const override;
    graphics::PTexture doDepthTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const override;
};

class ShadowFrameBufferCube : public ShadowFrameBuffer
{
public:
    ShadowFrameBufferCube(const std::shared_ptr<graphics::IRenderer>&);

protected:
    graphics::PTexture doColorTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const override;
    graphics::PTexture doDepthTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&) const override;
};

}
}

#endif // FRAMEBUFFERHELPERS_H
