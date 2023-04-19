#ifndef QTRENDERWIDGETPRIVATE_H
#define QTRENDERWIDGETPRIVATE_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace qt
{

class QtOpenGL_4_5_Renderer;

class QtRenderWidgetPrivate
{
public:
    QtRenderWidgetPrivate() {}
    ~QtRenderWidgetPrivate() {}

    std::shared_ptr<QtOpenGL_4_5_Renderer> &renderer() { return m_renderer; }
    std::weak_ptr<core::IApplication> &application() { return m_application; }

    uint64_t &startTime() { return m_startTime; }
    uint64_t &lastUpdateTime() { return m_lastUpdateTime; }
    uint64_t &lastFpsTime() { return m_lastFpsTime; }
    uint32_t &fpsCounter() { return m_fpsCounter; }

private:
    std::shared_ptr<QtOpenGL_4_5_Renderer> m_renderer;
    std::weak_ptr<core::IApplication> m_application;
    uint64_t m_startTime;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastFpsTime;
    uint32_t m_fpsCounter;
};

}
}

#endif // QTRENDERWIDGETPRIVATE_H
