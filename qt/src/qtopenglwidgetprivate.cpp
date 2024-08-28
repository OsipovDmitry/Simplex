#include "qtopenglwidgetprivate.h"

namespace simplex
{
namespace qt
{

std::unordered_set<std::weak_ptr<QtOpenGLWidget>, QtOpenGLWidgetHash> QtOpenGLWidgetPrivate::m_instances;

QtOpenGLWidgetPrivate::QtOpenGLWidgetPrivate(const std::string &name)
    : m_name(name)
{
}

QtOpenGLWidgetPrivate::~QtOpenGLWidgetPrivate() = default;

std::string &QtOpenGLWidgetPrivate::name()
{
    return m_name;
}

std::weak_ptr<core::ApplicationBase> &QtOpenGLWidgetPrivate::application()
{
    return m_application;
}

std::shared_ptr<QtOpenGL_4_5_Renderer> &QtOpenGLWidgetPrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<DefaultFrameBuffer_4_5> &QtOpenGLWidgetPrivate::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

uint64_t &QtOpenGLWidgetPrivate::startTime()
{
    return m_startTime;
}

uint64_t &QtOpenGLWidgetPrivate::lastUpdateTime()
{
    return m_lastUpdateTime;
}

uint64_t &QtOpenGLWidgetPrivate::lastFpsTime()
{
    return m_lastFpsTime;
}

uint32_t &QtOpenGLWidgetPrivate::fpsCounter()
{
    return m_fpsCounter;
}

std::unordered_set<std::weak_ptr<QtOpenGLWidget>, QtOpenGLWidgetHash> &QtOpenGLWidgetPrivate::instances()
{
    return m_instances;
}

}
}
