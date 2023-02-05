#include <core/iapplication.h>

#include <qt/qtrenderwidget.h>
#include <qt/qtopengl_1_0_renderer.h>

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
    format.setVersion(1, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);

    setFormat(format);
}

QtRenderWidget::~QtRenderWidget() = default;

void QtRenderWidget::setApplication(std::shared_ptr<core::IApplication> application)
{
    m_->application = application;
}

std::shared_ptr<core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer()
{
    return m_->renderer;
}

std::shared_ptr<const core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer() const
{
    return m_->renderer;
}

void QtRenderWidget::initializeGL()
{
    m_->renderer = std::make_shared<QtOpenGL_1_0_Renderer>(context());
}

void QtRenderWidget::resizeGL(int width, int height)
{
    m_->renderer->resize(width, height);
}

void QtRenderWidget::paintGL()
{
    if (!m_->application.expired())
        m_->application.lock()->update();

    m_->renderer->render();
}

}
}