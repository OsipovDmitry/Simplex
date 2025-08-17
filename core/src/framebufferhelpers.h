#ifndef FRAMEBUFFERHELPERS_H
#define FRAMEBUFFERHELPERS_H

#include <utils/glm/vec2.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class GeometryBuffer;
class ShadowBuffer;

class FrameBufferWrapper
{
public:
    FrameBufferWrapper(const std::shared_ptr<graphics::IFrameBuffer>&);
    virtual ~FrameBufferWrapper();

    const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer() const;
    const glm::uvec2 &viewportSize() const;

protected:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    glm::uvec2 m_viewportSize;
};

class GFrameBufferTmp
{
public:
    GFrameBufferTmp(const std::shared_ptr<graphics::IFrameBuffer>&, const std::shared_ptr<GeometryBuffer>&);

    void setForGeometryOpaquePass();
    void setForGeometryTransparentPass();

    void setForStencilPass();
    void setForLightPass();

    void setForBackgroundPass();
    void setForFinalPass();

    std::shared_ptr<graphics::IFrameBuffer> frameBuffer() const;

private:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<GeometryBuffer> m_GBuffer;
};

class PostprocessFrameBuffer
{
public:
    PostprocessFrameBuffer(const std::shared_ptr<graphics::IFrameBuffer>&);

    void setForPass();

    std::shared_ptr<graphics::IFrameBuffer> frameBuffer() const;

private:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
};

class ShadowFrameBuffer
{
public:
    ShadowFrameBuffer(
        const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
        const std::shared_ptr<ShadowBuffer>& shadowBuffer);

    void setForOpaquePass();
    void setForTransparentPass();

    std::shared_ptr<graphics::IFrameBuffer> frameBuffer() const;
    std::shared_ptr<ShadowBuffer> shadowBuffer() const;


private:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<ShadowBuffer> m_shadowBuffer;

    ShadingMode m_shadingMode;
    ShadingFilter m_shadingFilter;
};

class SSAOFrameBuffer : public FrameBufferWrapper
{
public:
    SSAOFrameBuffer(const std::shared_ptr<graphics::RendererBase>&);

    void resize(const std::shared_ptr<graphics::RendererBase>&, const glm::uvec2&, SSAOMode);
    graphics::PConstTexture colorTexture() const;
    graphics::PixelInternalFormat pixelInternalFormat() const;

protected:
    graphics::PixelInternalFormat m_pixelInternalFormat;

};

class BlurFrameBuffer : public FrameBufferWrapper
{
public:
    BlurFrameBuffer(const std::shared_ptr<graphics::RendererBase>&);

    void resize(const std::shared_ptr<graphics::RendererBase>&, const glm::uvec2&, graphics::PixelInternalFormat);
    graphics::PConstTexture colorTexture() const;

protected:
    graphics::PixelInternalFormat m_pixelInternalFormat;

};

}
}

#endif // FRAMEBUFFERHELPERS_H
