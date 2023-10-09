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

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    graphics::PConstTexture color0Texture() const;
    graphics::PConstTexture color1Texture() const;
};

class OITFrameBuffer : public FrameBufferWrapper
{
public:
    OITFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    const graphics::PConstImage &oitIndicesImage();

private:
    graphics::PConstImage m_oitIndicesImage;
};

class FinalFrameBuffer : public FrameBufferWrapper
{
public:
    FinalFrameBuffer(const std::shared_ptr<graphics::IRenderer>&);

    void setForBackgroundPass();
    void setForStencilPass();
    void setForLightPass();

    void resize(const std::shared_ptr<graphics::IRenderer>&, const graphics::PConstTexture&, const glm::uvec2&);
    graphics::PConstTexture colorTexture() const;
};

}
}

#endif // FRAMEBUFFERHELPERS_H
