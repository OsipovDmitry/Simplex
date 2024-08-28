#ifndef FRAMEBUFFERHELPERS_H
#define FRAMEBUFFERHELPERS_H

#include <utils/glm/vec2.hpp>

#include <core/forwarddecl.h>

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
    const glm::uvec2 &viewportSize() const;

protected:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    glm::uvec2 m_viewportSize;
};

class GFrameBuffer : public FrameBufferWrapper
{
public:
    GFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void setForGeometryOpaquePass();
    void setForGeometryTransparentPass();

    void setForStencilPass();
    void setForLightPass();

    void setForBackgroundPass();
    void setForFinalPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&);
    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture stencilTexture() const;
    graphics::PConstImage oitDepthImage();
    graphics::PConstImage oitIndicesImage();

private:
    graphics::PConstImage m_oitDepthImage;
    graphics::PConstImage m_oitIndicesImage;
};

class PostprocessFrameBuffer : public FrameBufferWrapper
{
public:
    PostprocessFrameBuffer(const std::shared_ptr<graphics::IRenderer>&, const std::shared_ptr<graphics::IFrameBuffer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&);

private:
    bool m_useUserFrameBuffer;
};

class ShadowFrameBuffer : public FrameBufferWrapper
{
public:
    ShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    uint32_t numLayers() const;

    void setForOpaquePass();
    void setForTransparentPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, ShadingMode, ShadingFilter);
    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture depthVSMTexture() const;

protected:
    virtual graphics::PTexture doTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, graphics::PixelInternalFormat) const = 0;

    ShadingMode m_shadingMode;
    ShadingFilter m_shadingFilter;
};

class ShadowFrameBuffer2D : public ShadowFrameBuffer
{
public:
    ShadowFrameBuffer2D(const std::shared_ptr<graphics::IRenderer>&);

protected:
    graphics::PTexture doTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, graphics::PixelInternalFormat) const override;
};

class ShadowFrameBufferCube : public ShadowFrameBuffer
{
public:
    ShadowFrameBufferCube(const std::shared_ptr<graphics::IRenderer>&);

protected:
    graphics::PTexture doTexture(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, graphics::PixelInternalFormat) const override;
};

class SSAOFrameBuffer : public FrameBufferWrapper
{
public:
    SSAOFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, SSAOMode);
    graphics::PConstTexture colorTexture() const;
    graphics::PixelInternalFormat pixelInternalFormat() const;

protected:
    graphics::PixelInternalFormat m_pixelInternalFormat;

};

class BlurFrameBuffer : public FrameBufferWrapper
{
public:
    BlurFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const glm::uvec2&, graphics::PixelInternalFormat);
    graphics::PConstTexture colorTexture() const;

protected:
    graphics::PixelInternalFormat m_pixelInternalFormat;

};

}
}

#endif // FRAMEBUFFERHELPERS_H
