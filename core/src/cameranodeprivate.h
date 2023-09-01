#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

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
    std::shared_ptr<graphics::IFrameBuffer> &GFrameBuffer();
    std::shared_ptr<graphics::IFrameBuffer> &OITFrameBuffer();
    std::shared_ptr<graphics::IFrameBuffer> &OITClearIndicesFrameBuffer();
    std::shared_ptr<graphics::IFrameBuffer> &OITSortNodesFrameBuffer();
    std::shared_ptr<graphics::IFrameBuffer> &finalFrameBuffer();
    std::shared_ptr<graphics::IImage> &OITIndicesImage();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_isRenderingEnabled;

    glm::uvec2 m_viewportSize;
    std::shared_ptr<graphics::IFrameBuffer> m_GFrameBuffer;
    std::shared_ptr<graphics::IFrameBuffer> m_OITFrameBuffer;
    std::shared_ptr<graphics::IFrameBuffer> m_OITClearIndicesFrameBuffer;
    std::shared_ptr<graphics::IFrameBuffer> m_OITSortNodesFrameBuffer;
    std::shared_ptr<graphics::IFrameBuffer> m_finalFrameBuffer;
    std::shared_ptr<graphics::IImage> m_OITIndicesImage;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
