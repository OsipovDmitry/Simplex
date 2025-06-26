#include <QDateTime>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QResizeEvent>

#include <utils/logger.h>

#include <core/applicationbase.h>

#include <qt/openglwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "openglwidgetprivate.h"

namespace simplex
{
namespace qt
{

OpenGLWidget::~OpenGLWidget()
{
    m_->renderer() = nullptr;

    delete m_->offscreenSurface();

    delete m_->context();

    LOG_INFO << "OpenGLWidget \"" << OpenGLWidget::name() << "\" has been destroyed";
}

const std::string &OpenGLWidget::name() const
{
    return m_->name();
}

QSurface *OpenGLWidget::surface() const
{
    return m_->offscreenSurface();
}

void OpenGLWidget::setApplication(const std::shared_ptr<core::ApplicationBase> &value)
{
    m_->application() = value;

    if (auto app = application())
        LOG_INFO << "Application \"" << app->name() << "\" has been set to \"" << name() << "\"";
    else
        LOG_INFO << "Application has been removed from \"" << name() << "\"";
}

std::shared_ptr<core::ApplicationBase> OpenGLWidget::application() const
{
    auto &applicationWeak = m_->application();
    return applicationWeak.expired() ? nullptr : applicationWeak.lock();
}

std::shared_ptr<core::graphics::IRenderer> OpenGLWidget::renderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::graphics::IRenderer> OpenGLWidget::renderer() const
{
    return const_cast<OpenGLWidget*>(this)->renderer();
}

std::shared_ptr<core::graphics::IFrameBuffer> OpenGLWidget::defaultFrameBuffer()
{
    return m().defaultFrameBuffer();
}

std::shared_ptr<const core::graphics::IFrameBuffer> OpenGLWidget::defaultFrameBuffer() const
{
    return const_cast<OpenGLWidget*>(this)->defaultFrameBuffer();
}

std::shared_ptr<OpenGLWidget> OpenGLWidget::getOrCreate(const std::string &name)
{
    std::shared_ptr<OpenGLWidget> result;

    auto &instances = OpenGLWidgetPrivate::instances();
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
        result = std::shared_ptr<OpenGLWidget>(new OpenGLWidget(name));
        OpenGLWidgetPrivate::instances().insert(result);
    }

    return result;
}

void OpenGLWidget::resizeEvent(QResizeEvent *e)
{
    auto width = e->size().width();
    auto height = e->size().height();

    m_->renderer()->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    LOG_INFO << "OpenGLWidget \"" << name() << "\" resized (" << width << ", " << height << ")";
}

void OpenGLWidget::paintEvent(QPaintEvent *)
{
    if (updatesEnabled())
    {
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
}

//void QtOpenGLWidget::resizeGL(int width, int height)
//{
//    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
//    {
//        width = pd->width();
//        height = pd->height();
//    }

//    LOG_INFO << "QtRenderWidget::resizeGL(" << width << ", " << height << ")";
//    if (auto &renderer = m_->renderer(); renderer)
//        renderer->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
//}

//void QtOpenGLWidget::paintGL()
//{
//    if (!m_->renderer())
//    {
//        m_->renderer() = QtOpenGL_4_5_Renderer::create(context());
//        m_->defaultFrameBuffer() = std::make_shared<DefaultFrameBuffer_4_5>(defaultFramebufferObject());

//        m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
//        m_->lastUpdateTime() = 0u;
//    }

//    auto time = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) - m_->startTime();
//    auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
//    m_->lastUpdateTime() = time;

//    static const uint64_t deltaFps = 1000u;
//    ++m_->fpsCounter();
//    if (time - m_->lastFpsTime() >= deltaFps)
//    {
//        auto lastFps = static_cast<uint32_t>(static_cast<float>(m_->fpsCounter()) / (0.001f * deltaFps) + 0.5f);
//        m_->fpsCounter() = 0u;
//        m_->lastFpsTime() = time;

//        //LOG_INFO << "FPS: " << lastFps;
//    }

//    if (auto app = application())
//        app->update(shared_from_this(), time, dt);
//}

OpenGLWidget::OpenGLWidget(const std::string &name)
    : QWidget()
    , m_(std::make_unique<OpenGLWidgetPrivate>(name))
{
    connect(this, &QObject::objectNameChanged, [this](const QString &value) { m().name() = value.toStdString(); } );
    setObjectName(QString::fromStdString(name));

    int major = 4, minor = 5;

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setVersion(major, minor);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

    auto &context = m_->context();
    context = new QOpenGLContext;
    context->setFormat(surfaceFormat);

    if (!context->create())
        LOG_CRITICAL << "Failed to create OpenGL context";

    auto &offscreenSurface = m_->offscreenSurface();
    offscreenSurface = new QOffscreenSurface;
    offscreenSurface->setFormat(context->format());
    offscreenSurface->setScreen(context->screen());
    offscreenSurface->create();

    context->makeCurrent(offscreenSurface);

    m_->renderer() = QtOpenGL_4_5_Renderer::create(context);
    //m_->defaultFrameBuffer() = std::make_shared<DefaultFrameBuffer_4_5>(defaultFramebufferObject());

    m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
    m_->lastUpdateTime() = 0u;

    LOG_INFO << "OpenGLWidget has been created (Name: \"" << OpenGLWidget::name() << "\", OpenGL context ver:" << major << "." << minor << ")";
}

}
}
