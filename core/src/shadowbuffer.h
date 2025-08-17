#ifndef CORE_SHADOWBUFFER_H
#define CORE_SHADOWBUFFER_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ShadowBuffer
{
public:
    ShadowBuffer();
    virtual ~ShadowBuffer();

    const glm::uvec2& size() const;
    ShadingMode shadingMode() const;
    ShadingFilter shadingFilter() const;
    void update(const glm::uvec2&, ShadingMode, ShadingFilter);

    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture depthVSMTexture() const;

protected:
    virtual graphics::PTexture doTexture(graphics::PixelInternalFormat, graphics::TextureFilterMode) const;

private:
    glm::uvec2 m_size;
    ShadingMode m_shadingMode;
    ShadingFilter m_shadingFilter;

    graphics::PTexture m_colorTexture;
    graphics::PTexture m_depthTexture;
    graphics::PTexture m_depthVSMTexture;
};

class ShadowBuffer2D : public ShadowBuffer
{
public:
    ShadowBuffer2D();
    ~ShadowBuffer2D() override;

protected:
    graphics::PTexture doTexture(graphics::PixelInternalFormat, graphics::TextureFilterMode) const override;
};

class ShadowBufferCube : public ShadowBuffer
{
public:
    ShadowBufferCube();
    ~ShadowBufferCube() override;

protected:
    graphics::PTexture doTexture(graphics::PixelInternalFormat, graphics::TextureFilterMode) const override;
};

}
}

#endif // CORE_SHADOWBUFFER_H
