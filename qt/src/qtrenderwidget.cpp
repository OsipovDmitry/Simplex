#include <QDateTime>
#include <QOpenGLPaintDevice>
#include <QCloseEvent>

#include <utils/logger.h>

#include <core/iapplication.h>

#include <qt/qtrenderwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "qtrenderwidgetprivate.h"

namespace simplex
{
namespace qt
{

QtRenderWidget::QtRenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_(std::make_unique<QtRenderWidgetPrivate>())
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    LOG_INFO << "QtRenderWidget has been created (OpenGL context ver:" << format.majorVersion() << "." << format.minorVersion() << ")";
}

QtRenderWidget::~QtRenderWidget()
{
    LOG_INFO << "QtRenderWidget has been destroyed";
}

void QtRenderWidget::setApplication(std::weak_ptr<core::IApplication> application)
{
    if (application.expired())
        LOG_CRITICAL << "Application can't be nullptr";

    m_->application() = application;
    LOG_INFO << "Application \"" + application.lock()->name() + "\" has been set to QtRenderWidget";
}

std::weak_ptr<core::IApplication> QtRenderWidget::application()
{
    return m_->application();
}

std::weak_ptr<const core::IApplication> QtRenderWidget::application() const
{
    return const_cast<QtRenderWidget*>(this)->application();
}

std::shared_ptr<core::graphics::IRenderer> QtRenderWidget::graphicsRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::graphics::IRenderer> QtRenderWidget::graphicsRenderer() const
{
    return const_cast<QtRenderWidget*>(this)->graphicsRenderer();
}

void QtRenderWidget::resizeGL(int width, int height)
{
    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
    {
        width = pd->width();
        height = pd->height();
    }

    LOG_INFO << "QtRenderWidget::resizeGL(" << width << ", " << height << ")";
    if (auto &renderer = m_->renderer(); renderer)
        renderer->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

void QtRenderWidget::paintGL()
{
    if (!m_->renderer())
    {
        m_->renderer() = QtOpenGL_4_5_Renderer::create(context(), defaultFramebufferObject());

        m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
        m_->lastUpdateTime() = 0u;
    }

    auto time = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) - m_->startTime();
    auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
    m_->lastUpdateTime() = time;

    static const uint64_t deltaFps = 1000;
    ++m_->fpsCounter();
    if (time - m_->lastFpsTime() >= deltaFps)
    {
        auto lastFps = static_cast<uint32_t>(static_cast<float>(m_->fpsCounter()) / (0.001f * deltaFps) + 0.5f);
        m_->fpsCounter() = 0u;
        m_->lastFpsTime() = time;

        //LOG_INFO << "FPS: " << lastFps;
    }

    if (auto app = m_->application(); !app.expired())
        app.lock()->update(time, dt);
}

void QtRenderWidget::closeEvent(QCloseEvent *event)
{
    makeCurrent();

    if (auto app = m_->application(); !app.expired())
        app.lock()->shutDown();

    m_->renderer() = nullptr;

    event->accept();
}

}
}
