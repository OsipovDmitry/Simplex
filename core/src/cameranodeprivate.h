#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>
#include <utils/range.h>

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
    utils::Range &cullPlanesLimits();
    std::shared_ptr<GFrameBuffer> &gFrameBuffer();
    std::shared_ptr<OITFrameBuffer> &oitFrameBuffer();
    std::shared_ptr<LightFrameBuffer> &lightFrameBuffer();
    std::shared_ptr<FinalFrameBuffer> &finalFrameBuffer();
    std::shared_ptr<PostprocessFrameBuffer> &postprocessFrameBuffer();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_isRenderingEnabled;

    glm::uvec2 m_viewportSize;
    utils::Range m_cullPlanesLimits;
    std::shared_ptr<GFrameBuffer> m_gFrameBuffer;
    std::shared_ptr<OITFrameBuffer> m_oitFrameBuffer;
    std::shared_ptr<LightFrameBuffer> m_lightFrameBuffer;
    std::shared_ptr<FinalFrameBuffer> m_finalFrameBuffer;
    std::shared_ptr<PostprocessFrameBuffer> m_postprocessFrameBuffer;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
