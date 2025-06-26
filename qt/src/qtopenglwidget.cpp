#include <QDateTime>
#include <QOpenGLPaintDevice>
#include <QCloseEvent>

#include <utils/logger.h>

#include <core/applicationbase.h>

#include <qt/qtopenglwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "qtopenglwidgetprivate.h"

namespace simplex
{
namespace qt
{

QtOpenGLWidget::~QtOpenGLWidget()
{
    m_->renderer() = nullptr;

    LOG_INFO << "QtOpenGLWidget \"" << QtOpenGLWidget::name() << "\" has been destroyed";
}

const std::string &QtOpenGLWidget::name() const
{
    return m_->name();
}

void QtOpenGLWidget::setApplication(const std::shared_ptr<core::ApplicationBase> &value)
{
    m_->application() = value;

    if (auto app = application())
        LOG_INFO << "Application \"" << app->name() << "\" has been set to \"" << name() << "\"";
    else
        LOG_INFO << "Application has been removed from \"" << name() << "\"";
}

std::shared_ptr<core::ApplicationBase> QtOpenGLWidget::application() const
{
    auto &applicationWeak = m_->application();
    return applicationWeak.expired() ? nullptr : applicationWeak.lock();
}

std::shared_ptr<core::graphics::IRenderer> QtOpenGLWidget::renderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::graphics::IRenderer> QtOpenGLWidget::renderer() const
{
    return const_cast<QtOpenGLWidget*>(this)->renderer();
}

std::shared_ptr<core::graphics::IFrameBuffer> QtOpenGLWidget::defaultFrameBuffer()
{
    return m().defaultFrameBuffer();
}

std::shared_ptr<const core::graphics::IFrameBuffer> QtOpenGLWidget::defaultFrameBuffer() const
{
    return const_cast<QtOpenGLWidget*>(this)->defaultFrameBuffer();
}

std::shared_ptr<QtOpenGLWidget> QtOpenGLWidget::getOrCreate(const std::string &name)
{
    std::shared_ptr<QtOpenGLWidget> result;

    auto &instances = QtOpenGLWidgetPrivate::instances();
    for (auto it = instances.begin(); it != instances.end(); )
    {
        if (it->expired())
            it = instances.erase(it);
        else if (auto widget = it->lock(); widget->name() == name)
        {
            result = widget;
            break;
        }
        else
            ++it;
    }

    if (!result)
    {
        result = std::shared_ptr<QtOpenGLWidget>(new QtOpenGLWidget(name));
        QtOpenGLWidgetPrivate::instances().insert(result);
    }

    return result;
}

void QtOpenGLWidget::resizeGL(int width, int height)
{
    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
    {
        width = pd->width();
        height = pd->height();
    }

    LOG_INFO << "QtOpenGLWidget::resizeGL(" << width << ", " << height << ")";
    if (auto &renderer = m_->renderer(); renderer)
        renderer->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

void QtOpenGLWidget::paintGL()
{
    if (!m_->renderer())
    {
        m_->renderer() = QtOpenGL_4_5_Renderer::create(context());
        m_->defaultFrameBuffer() = std::make_shared<DefaultFrameBuffer_4_5>(defaultFramebufferObject());

        m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
        m_->lastUpdateTime() = 0u;
    }

    auto time = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) - m_->startTime();
    auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
    m_->lastUpdateTime() = time;

    static const uint64_t deltaFps = 1000u;
    ++m_->fpsCounter();
    if (time - m_->lastFpsTime() >= deltaFps)
    {
        auto lastFps = static_cast<uint32_t>(static_cast<float>(m_->fpsCounter()) / (0.001f * deltaFps) + 0.5f);
        m_->fpsCounter() = 0u;
        m_->lastFpsTime() = time;

        //LOG_INFO << "FPS: " << lastFps;
    }

    if (auto app = application())
        app->update(shared_from_this(), time, dt);
}

QtOpenGLWidget::QtOpenGLWidget(const std::string &name)
    : QOpenGLWidget()
    , m_(std::make_unique<QtOpenGLWidgetPrivate>(name))
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    connect(this, &QObject::objectNameChanged, [this](const QString &value) { m().name() = value.toStdString(); } );
    setObjectName(QString::fromStdString(name));

    int major = -1, minor = -1;
    major = format.majorVersion();
    minor = format.minorVersion();

    LOG_INFO << "QtOpenGLWidget has been created (Name: \"" << QtOpenGLWidget::name() << "\", OpenGL context ver:" << major << "." << minor << ")";
}

}
}
