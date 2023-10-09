#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include <core/forwarddecl.h>

#include "nodeprivate.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(const std::string&);
    ~CameraNodePrivate() override;

    std::shared_ptr<utils::AbstractClipSpace> &clipSpace();
    bool &isRenderingEnabled();

    glm::uvec2 &viewportSize();
    glm::vec2 &cullPlanesLimits();
    graphics::PConstTexture &sharedDepthStencilTexture();
    std::shared_ptr<GFrameBuffer> &gFrameBuffer();
    std::shared_ptr<OITFrameBuffer> &oitFrameBuffer();
    std::shared_ptr<FinalFrameBuffer> &finalFrameBuffer();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_isRenderingEnabled;

    glm::uvec2 m_viewportSize;
    glm::vec2 m_cullPlanesLimits;
    graphics::PConstTexture m_sharedDepthStencilTexture;
    std::shared_ptr<GFrameBuffer> m_gFrameBuffer;
    std::shared_ptr<OITFrameBuffer> m_oitFrameBuffer;
    std::shared_ptr<FinalFrameBuffer> m_finalFrameBuffer;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
