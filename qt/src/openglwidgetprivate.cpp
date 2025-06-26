#include <utils/logger.h>

#include "openglwidgetprivate.h"

namespace simplex
{
namespace qt
{

std::unordered_set<std::weak_ptr<OpenGLWidget>, OpenGLWidgetHash> OpenGLWidgetPrivate::m_instances;

OpenGLWidgetPrivate::OpenGLWidgetPrivate(const std::string &name)
    : m_name(name)
    , m_surfaceFormat()
{
}

OpenGLWidgetPrivate::~OpenGLWidgetPrivate() = default;

std::string &OpenGLWidgetPrivate::name()
{
    return m_name;
}

std::weak_ptr<core::ApplicationBase> &OpenGLWidgetPrivate::application()
{
    return m_application;
}

std::shared_ptr<QtOpenGL_4_5_Renderer> &OpenGLWidgetPrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<DefaultFrameBuffer_4_5> &OpenGLWidgetPrivate::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

uint64_t &OpenGLWidgetPrivate::startTime()
{
    return m_startTime;
}

uint64_t &OpenGLWidgetPrivate::lastUpdateTime()
{
    return m_lastUpdateTime;
}

uint64_t &OpenGLWidgetPrivate::lastFpsTime()
{
    return m_lastFpsTime;
}

uint32_t &OpenGLWidgetPrivate::fpsCounter()
{
    return m_fpsCounter;
}

QOpenGLContext *&OpenGLWidgetPrivate::context()
{
    return m_context;
}

QSurfaceFormat &OpenGLWidgetPrivate::surfaceFormat()
{
    return m_surfaceFormat;
}

QOffscreenSurface *&OpenGLWidgetPrivate::offscreenSurface()
{
    return m_offscreenSurface;
}

std::unordered_set<std::weak_ptr<OpenGLWidget>, OpenGLWidgetHash> &OpenGLWidgetPrivate::instances()
{
    return m_instances;
}

}
}
