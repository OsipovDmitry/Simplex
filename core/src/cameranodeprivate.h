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
    CameraNodePrivate(const std::string &name);

    std::shared_ptr<utils::AbstractClipSpace> &clipSpace();
    bool &renderingEnabled();

    std::shared_ptr<ColoredDrawable> &cameraDrawable();
    std::shared_ptr<ColoredDrawable> &frustumDrawable();

    static std::shared_ptr<IGraphicsRenderer::VertexArray> &cameraVertexArray();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_renderingEnabled;

    std::shared_ptr<ColoredDrawable> m_cameraDrawable;
    std::shared_ptr<ColoredDrawable> m_frustumDrawable;

    static std::shared_ptr<IGraphicsRenderer::VertexArray> s_cameraVertexArray;

};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
