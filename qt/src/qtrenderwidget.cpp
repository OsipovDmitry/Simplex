#include <QDateTime>

#include <utils/logger.h>

#include <core/iapplication.h>

#include <qt/qtrenderwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "qtrenderwidgetprivate.h"

//tmp
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/glm/gtc/matrix_transform.hpp>

std::shared_ptr<simplex::core::IGraphicsRenderer::VertexArray> vao;
std::shared_ptr<simplex::core::IGraphicsRenderer::RenderProgram> renderProgram;

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
    m_->setApplication(application);
    LOG_INFO << "Application \"" + application->name() + "\" has been set to QtRenderWidget";
}

std::shared_ptr<core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::IGraphicsRenderer> QtRenderWidget::graphicsRenderer() const
{
    return m_->renderer();
}

void QtRenderWidget::initializeGL()
{
    LOG_INFO << "QtRenderWidget::initializeGL()";

    m_->setRenderer(QtOpenGL_4_5_Renderer::instance());
    if (!m_->renderer())
    {
        m_->setRenderer(std::shared_ptr<QtOpenGL_4_5_Renderer>(new QtOpenGL_4_5_Renderer(context())));
        QtOpenGL_4_5_Renderer::setInstance(m_->renderer());
    }

    m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
    m_->lastUpdateTime() = 0u;

    // tmp
    auto mesh = utils::Mesh::createEmptyMesh(utils::PrimitiveType::Triangles,
                                             {{utils::VertexAttribute::Position, {3, utils::Type::Single}},
                                              {utils::VertexAttribute::Normal, {3, utils::Type::Single}},
                                              {utils::VertexAttribute::Color, {3, utils::Type::Single}}},
                                             utils::Type::Uint32);
    utils::MeshPainter painter(mesh);
    painter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    painter.setVertexTransform(glm::translate(glm::mat4(1.f), glm::vec3(-.5f, .2f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f)));
    painter.drawTeapot();
    painter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    painter.setVertexTransform(glm::translate(glm::mat4(1.f), glm::vec3(+.5f, .3f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f)));
    painter.drawMonkey();
    painter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    painter.setVertexTransform(glm::translate(glm::mat4(1.f), glm::vec3(-.5f, -.5f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f)));
    painter.drawCube();
    painter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    painter.setVertexTransform(glm::translate(glm::mat4(1.f), glm::vec3(+.5f, -.5f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(0.7f)));
    painter.drawTriangle();
    vao = m_->renderer()->createVertexArray(mesh);

    const std::string vertexShaderSource = {
        "#version 450\n"
        "layout(location = 0) in vec3 iposition;\n"
        "layout(location = 1) in vec3 inormal;\n"
        "layout(location = 6) in vec3 icolor;\n"
        "out vec3 normal;"
        "out vec4 color;"
        "void main() {\n"
        "   normal = inormal;\n"
        "   color = vec4(icolor, 1.0f);\n"
        "   gl_Position = vec4(iposition, 1.0f);\n"
        "}"
    };

    const std::string fragmentShaderSource = {
        "#version 450\n"
        "in vec3 normal;"
        "in vec4 color;\n"
        "layout(location = 0) out vec4 ocolor;\n"
        "void main() {\n"
        "  ocolor = color;\n"
        "}"
    };

    renderProgram = m_->renderer()->createRenderProgram(vertexShaderSource, fragmentShaderSource);
}

void QtRenderWidget::resizeGL(int width, int height)
{
    LOG_INFO << "QtRenderWidget::resizeGL(" << width << ", " << height << ")";
    m_->renderer()->resize(width, height);
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

        LOG_INFO << "FPS: " << lastFps;
    }

    if (auto app = m_->application(); app)
        app->update(time, dt);

    m_->renderer()->render();

    // tmp
    m_->renderer()->render2(renderProgram, vao);
}

}
}
