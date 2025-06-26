#ifndef QT_OPENGLWIDGETPRIVATE_H
#define QT_OPENGLWIDGETPRIVATE_H

#include <memory>
#include <string>

#include <QSurfaceFormat>

#include <qt/openglwidget.h>

class QOpenGLContext;
class QOffscreenSurface;

namespace simplex
{
namespace qt
{

inline bool operator ==(const std::weak_ptr<OpenGLWidget>& l, const std::weak_ptr<OpenGLWidget>& r)
{
    if (l.expired() || r.expired())
        return false;

    return l.lock() == r.lock();
}

struct OpenGLWidgetHash
{
public:
    size_t operator ()(const std::weak_ptr<OpenGLWidget>& value) const
    {
        return value.expired() ? 0u : std::hash<std::string>()(value.lock()->name());
    }
};

class QtOpenGL_4_5_Renderer;
class DefaultFrameBuffer_4_5;

class OpenGLWidgetPrivate
{
public:
    OpenGLWidgetPrivate(const std::string &name);
    ~OpenGLWidgetPrivate();

    std::string &name();

    std::weak_ptr<core::ApplicationBase> &application();
    std::shared_ptr<QtOpenGL_4_5_Renderer> &renderer();
    std::shared_ptr<DefaultFrameBuffer_4_5> &defaultFrameBuffer();

    uint64_t &startTime();
    uint64_t &lastUpdateTime();
    uint64_t &lastFpsTime();
    uint32_t &fpsCounter();

    QOpenGLContext *&context();
    QSurfaceFormat &surfaceFormat();
    QOffscreenSurface *&offscreenSurface();

    static std::unordered_set<std::weak_ptr<OpenGLWidget>, OpenGLWidgetHash> &instances();

private:
    std::string m_name;
    std::weak_ptr<core::ApplicationBase> m_application;
    std::shared_ptr<QtOpenGL_4_5_Renderer> m_renderer;
    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;
    uint64_t m_startTime;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastFpsTime;
    uint32_t m_fpsCounter;

    QOpenGLContext *m_context;
    QSurfaceFormat m_surfaceFormat;
    QOffscreenSurface *m_offscreenSurface;

    static std::unordered_set<std::weak_ptr<OpenGLWidget>, OpenGLWidgetHash> m_instances;
};

}
}

#endif // QT_OPENGLWIDGETPRIVATE_H
