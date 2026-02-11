#ifndef CORE_BLUR_H
#define CORE_BLUR_H

#include <utils/glm/vec3.hpp>

#include <core/stateset.h>

namespace simplex
{
namespace core
{

class Blur : public StateSet, public std::enable_shared_from_this<Blur>
{
public:
    Blur();
    ~Blur() override;

    float sigma() const;
    void setSigma(float);

    graphics::PConstTexture sourceMap() const;
    uint32_t sourceMapLevel() const;
    void setSourceMap(const graphics::PConstTexture&, uint32_t, bool canDirtyMap = false);

    graphics::PConstTexture run(const std::shared_ptr<graphics::RendererBase>&, const std::shared_ptr<ProgramsLoader>&);

private:
    graphics::PConstBufferRange kernelBuffer();
    void setKernelBuffer(const graphics::PConstBufferRange&);

    float m_sigma;
    std::array<std::pair<graphics::PConstTexture, uint32_t>, 2u> m_pingpongSourceMap;

    graphics::PConstImage sourceImage() const;
    void setSourceImage(const graphics::PConstImage&);

    graphics::PConstImage destinationImage() const;
    void setDestinationImage(const graphics::PConstImage&);

    void setPassIndex(uint32_t);

    void checkMapsInvariants() const;
};

}
}

#endif // CORE_BLUR_H
