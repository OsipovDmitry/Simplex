#ifndef QT_QTOPENGLWIDGETPRIVATE_H
#define QT_QTOPENGLWIDGETPRIVATE_H

#include <memory>
#include <string>

#include <qt/qtopenglwidget.h>

namespace simplex
{
namespace qt
{

inline bool operator ==(const std::weak_ptr<QtOpenGLWidget>& l, const std::weak_ptr<QtOpenGLWidget>& r)
{
    if (l.expired() || r.expired())
        return false;

    return l.lock() == r.lock();
}

struct QtOpenGLWidgetHash
{
public:
    size_t operator ()(const std::weak_ptr<QtOpenGLWidget>& value) const
    {
        return value.expired() ? 0u : std::hash<std::string>()(value.lock()->name());
    }
};

class QtOpenGL_4_5_Renderer;
class DefaultFrameBuffer_4_5;

class QtOpenGLWidgetPrivate
{
public:
    QtOpenGLWidgetPrivate(const std::string &name);
    ~QtOpenGLWidgetPrivate();

    std::string &name();

    std::weak_ptr<core::ApplicationBase> &application();
    std::shared_ptr<QtOpenGL_4_5_Renderer> &renderer();
    std::shared_ptr<DefaultFrameBuffer_4_5> &defaultFrameBuffer();

    uint64_t &startTime();
    uint64_t &lastUpdateTime();
    uint64_t &lastFpsTime();
    uint32_t &fpsCounter();

    static std::unordered_set<std::weak_ptr<QtOpenGLWidget>, QtOpenGLWidgetHash> &instances();

private:
    std::string m_name;
    std::weak_ptr<core::ApplicationBase> m_application;
    std::shared_ptr<QtOpenGL_4_5_Renderer> m_renderer;
    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;
    uint64_t m_startTime;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastFpsTime;
    uint32_t m_fpsCounter;

    static std::unordered_set<std::weak_ptr<QtOpenGLWidget>, QtOpenGLWidgetHash> m_instances;
};

}
}

#endif // QT_QTOPENGLWIDGETPRIVATE_H
