#ifndef CORE_SSAODRAWABLE_H
#define CORE_SSAODRAWABLE_H

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class SSAODrawable : public Drawable
{
public:
    SSAODrawable(const std::shared_ptr<graphics::IVertexArray>&);
    ~SSAODrawable() override;

    float contribution() const;
    void setContribution(float);

    uint32_t kernelBufferSize() const;
    void setKernelBufferSize(uint32_t);

    uint32_t noiseTextureSize() const;
    void setNoiseTextureSize(uint32_t);

    float radius() const;
    void setRadius(float);

    void update(const std::shared_ptr<graphics::RendererBase>&);

private:
    graphics::PConstBufferRange kernelBuffer() const;
    void setKernelBuffer(const graphics::PConstBufferRange&);

    graphics::PConstTexture noiseTexture() const;
    void setNoiseTexture(const graphics::PConstTexture&);

    uint32_t m_kernelBufferSize;
    uint32_t m_noiseTextureSize;
};

}
}

#endif // CORE_SSAODRAWABLE_H
