#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include <core/forwarddecl.h>
#include <core/igraphicsrenderer.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(const std::string &name);
    ~CameraNodePrivate() override;

    std::shared_ptr<utils::AbstractClipSpace> &clipSpace();
    bool &renderingEnabled();

    std::shared_ptr<Viewport> &viewport();
    std::shared_ptr<IGraphicsRenderer::FrameBuffer> &frameBuffer();

    std::shared_ptr<ColoredDrawable> &cameraDrawable();
    std::shared_ptr<ColoredDrawable> &frustumDrawable();

    static std::shared_ptr<IGraphicsRenderer::VertexArray> &cameraVertexArray();
    static std::shared_ptr<IGraphicsRenderer::FrameBuffer> &defaultFrameBuffer();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_renderingEnabled;

    std::shared_ptr<Viewport> m_viewport;
    std::shared_ptr<IGraphicsRenderer::FrameBuffer> m_frameBuffer;

    std::shared_ptr<ColoredDrawable> m_cameraDrawable;
    std::shared_ptr<ColoredDrawable> m_frustumDrawable;

    static std::shared_ptr<IGraphicsRenderer::VertexArray> s_cameraVertexArray;
    static std::shared_ptr<IGraphicsRenderer::FrameBuffer> s_defaultFrameBuffer;

};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
