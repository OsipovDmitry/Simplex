#include <utils/logger.h>

#include <core/iapplication.h>

#include <qt/qtrenderwidget.h>

#include "qtopengl_4_5_renderer.h"
#include "qtrenderwidgetprivate.h"

////tmp
//#include <utils/types.h>

namespace simplex
{
namespace qt
{

////tmp
//static std::shared_ptr<core::IGraphicsRenderer::RenderProgram> renderProgram;
//static std::shared_ptr<core::IGraphicsRenderer::VertexArray> vertexArray;
GLuint shaderProgram;
GLuint vao;

QtRenderWidget::QtRenderWidget()
    : QOpenGLWidget(nullptr)
    , m_(std::make_unique<QtRenderWidgetPrivate>())
{
//    QSurfaceFormat format;
//    //format.setDepthBufferSize(24);
//    //format.setStencilBufferSize(8);
//    format.setVersion(4, 5);
//    format.setProfile(QSurfaceFormat::CoreProfile);

//    setFormat(format);

    //LOG_INFO << "QtRenderWidget has been created (OpenGL context ver:" << format.majorVersion() << "." << format.minorVersion() << ")";
}

QtRenderWidget::~QtRenderWidget()
{
    LOG_INFO << "QtRenderWidget has been destroyed";
}

void QtRenderWidget::setApplication(std::shared_ptr<core::IApplication> application)
{
    m_->application = application;
    LOG_INFO << "Application \"" + application->name() + "\" has been set to QtRenderWidget";
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
    LOG_INFO << "QtRenderWidget::initializeGL()";

    m_->renderer = QtOpenGL_4_5_Renderer::instance();
    if (!m_->renderer)
    {
        m_->renderer = std::shared_ptr<QtOpenGL_4_5_Renderer>(new QtOpenGL_4_5_Renderer(context()));
        QtOpenGL_4_5_Renderer::setInstance(m_->renderer);
    }

//    const std::string vSh(
//                "#version 450\n"
//                "layout(location = 0) in vec3 a_position;\n"
//                "layout(location = 1) in vec3 a_color;\n"
//                "out vec4 v_color;"
//                "void main() {\n"
//                "v_color = vec4(a_color, 1.0f);\n"
//                "gl_Position = vec4(a_position, 1.0f);\n"
//                "}");

//    const std::string fSh(
//                "#version 450\n"
//                "in vec4 v_color;\n"
//                "layout(location = 0) out vec4 o_color;\n"
//                "void main() {\n"
//                "  o_color = v_color;\n"
//                "}");

//    LOG_INFO << "Loading shader..";
//    renderProgram = graphicsRenderer()->createRenderProgram(vSh, fSh);
//    if (renderProgram) LOG_INFO << "Loading is Ok!";

//    std::vector<float> vertices {
//        -.5f, -.7f, 0.f,
//        .5f, -.7f, 0.f,
//        0.f, .5f, 0.f
//    };
//    std::vector<float> colors {
//        1.f, 0.f, 0.f,
//        0.f, 1.f, 0.f,
//        0.f, 0.f, 1.f
//    };

//    auto vBuffer = graphicsRenderer()->createBuffer(vertices.size() * sizeof(float), vertices.data());
//    auto cBuffer = graphicsRenderer()->createBuffer(colors.size() * sizeof(float), colors.data());

//    vertexArray = graphicsRenderer()->createVertexArray();
//    vertexArray->attachVertexBuffer(static_cast<utils::VertexAttribute>(0), vBuffer, 3, utils::Type::Single, 0, 12, 0);
//    vertexArray->attachVertexBuffer(static_cast<utils::VertexAttribute>(1), cBuffer, 3, utils::Type::Single, 0, 12, 0);

    auto r = m_->renderer;
    GLuint vbo {};
    r->glCreateBuffers(1, &vbo);

    const std::vector<float> vertices {
        -.5f, -.7f, 0.f,  1.0f, 0.0f, 0.0f,
        .5f, -.7f, 0.f,  0.0f, 1.0f, 0.0f,
        0.f, .5f, 0.f,  0.0f, 0.0f, 1.0f
    };

    r->glNamedBufferStorage(vbo, vertices.size() * sizeof(float), vertices.data(), 0);

    GLuint vertexShader = r->glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSource = {
        "#version 450\n"
        "layout(location = 0) in vec3 iposition;\n"
        "layout(location = 1) in vec3 icolor;\n"
        "out vec4 color;"
        "void main() {\n"
        "color = vec4(icolor, 1.0f);\n"
        "gl_Position = vec4(iposition, 1.0f);\n"
        "}"
    };

    r->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    r->glCompileShader(vertexShader);
    GLint compiled;
    r->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        r->glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
            r->glGetShaderInfoLog(vertexShader, infoLen, nullptr, infoLog);
            LOG_ERROR << "vertex shader compile error: " << infoLog;
            free(infoLog);
        }
    }

    GLuint fragmentShader = r->glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSource = {
        "#version 450\n"
        "in vec4 color;\n"
        "layout(location = 0) out vec4 ocolor;\n"
        "void main() {\n"
        "  ocolor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
        "}"
    };

    r->glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    r->glCompileShader(fragmentShader);

    r->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        r->glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
            r->glGetShaderInfoLog(fragmentShader, infoLen, nullptr, infoLog);
            LOG_ERROR << "fragment shader compile error: " << infoLog;
            free(infoLog);
        }
    }

    GLuint shaderProgram = r->glCreateProgram();

    r->glAttachShader(shaderProgram, vertexShader);
    r->glAttachShader(shaderProgram, fragmentShader);
    r->glLinkProgram(shaderProgram);

    GLint linked;
    r->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        r->glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
            r->glGetProgramInfoLog(shaderProgram, infoLen, nullptr, infoLog);
            LOG_ERROR << "Program link error: " << infoLog;
            free(infoLog);
        }
    }

    r->glCreateVertexArrays(1, &vao);
    r->glVertexArrayVertexBuffer(vao, 0, vbo, 0, 6 * sizeof(float));
    r->glVertexArrayAttribBinding(vao, 0, 0);
    //r->glVertexArrayAttribBinding(vao, 1, 0);
    r->glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
    //r->glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    //r->glVertexArrayBindingDivisor(vao, 0, 0);
    r->glEnableVertexArrayAttrib(vao, 0);
    //r->glEnableVertexArrayAttrib(vao, 1);
}

void QtRenderWidget::resizeGL(int width, int height)
{
    LOG_INFO << "QtRenderWidget::resizeGL(" << width << ", " << height << ")";
    m_->renderer->resize(width, height);
}

void QtRenderWidget::paintGL()
{
    if (!m_->application.expired())
        m_->application.lock()->update();

    m_->renderer->render();

//    m_->renderer->render2(renderProgram, vertexArray);

    auto r = m_->renderer;

    r->glBindVertexArray(vao);
    r->glUseProgram(shaderProgram);
    r->glDrawArrays(GL_TRIANGLES, 0, 3);
}

}
}
