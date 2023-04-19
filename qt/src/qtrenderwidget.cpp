#include <QDateTime>
#include <QOpenGLPaintDevice>

#include <utils/logger.h>

#include <core/iapplication.h>

#include <qt/qtrenderwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "qtrenderwidgetprivate.h"

namespace simplex
{
namespace qt
{

QtRenderWidget::QtRenderWidget()
    : QOpenGLWidget(nullptr)
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

void QtRenderWidget::setApplication(std::shared_ptr<core::IApplication> application)
{
    m_->application() = application;
    LOG_INFO << "Application \"" + application->name() + "\" has been set to QtRenderWidget";
}

std::shared_ptr<core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer() const
{
    return const_cast<QtRenderWidget*>(this)->graphicsRenderer();
}

void QtRenderWidget::initializeGL()
{
    LOG_INFO << "QtRenderWidget::initializeGL()";

    m_->renderer() = QtOpenGL_4_5_Renderer::instance();
    if (!m_->renderer())
    {
        m_->renderer() = std::shared_ptr<QtOpenGL_4_5_Renderer>(new QtOpenGL_4_5_Renderer(context()));
        QtOpenGL_4_5_Renderer::setInstance(m_->renderer());
    }

    m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
    m_->lastUpdateTime() = 0u;
}

void QtRenderWidget::resizeGL(int width, int height)
{
    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
    {
        width = pd->width();
        height = pd->height();
    }

    LOG_INFO << "QtRenderWidget::resizeGL(" << width << ", " << height << ")";
    m_->renderer()->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

void QtRenderWidget::paintGL()
{
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

}
}
