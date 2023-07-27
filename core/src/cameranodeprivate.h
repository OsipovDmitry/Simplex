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

    std::shared_ptr<RenderSurface> &renderSurface();
    std::shared_ptr<GSurface> &gSurface();

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_isRenderingEnabled;

    std::shared_ptr<RenderSurface> m_renderSurface;
    std::shared_ptr<GSurface> m_gSurface;

};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
