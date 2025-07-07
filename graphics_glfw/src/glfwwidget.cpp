#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/logger.h>

#include <core/applicationbase.h>

#include <graphics_glfw/glfwwidget.h>

#include "glfwrenderer.h"
#include "glfwwidgetprivate.h"

namespace simplex
{
namespace graphics_glfw
{

static std::shared_ptr<GLFWWidget> findGLFWWidget(GLFWwindow* window)
{
    std::shared_ptr<GLFWWidget> result;
    for (const auto& widgetWeak : GLFWWidgetPrivate::instances())
    {
        if (widgetWeak.expired())
            continue;

        auto widget = widgetWeak.lock();
        if (widget->m().window() == window)
        {
            result = widget;
            break;
        }
    }

    return result;
}

static void errorCallback(int error, const char* description)
{
    LOG_ERROR << description;
}

static void closeCallback(GLFWwindow *window)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    auto& widgetPrivate = widget->m();

    GLFWWidgetPrivate::instances().erase(widget);
    widgetPrivate.shareGroup()->remove(widget);

    widgetPrivate.renderer() = nullptr;

    glfwDestroyWindow(widget->m().window());
    widgetPrivate.window() = nullptr;

    LOG_INFO << "GLFWWidget \"" << widget->name() << "\" has been destroyed";
}

static void sizeCallback(GLFWwindow* window, int width, int height)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    glfwGetFramebufferSize(window, &width, &height);
    
    LOG_INFO << "GLFWWidget resized(" << width << ", " << height << ")";
    if (auto &rend = widget->renderer(); rend)
        rend->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

GLFWWidget::~GLFWWidget()
{
    if (m_->window())
        closeCallback(m_->window());
}

const std::string & GLFWWidget::name() const
{
    return m_->name();
}

std::string GLFWWidget::title() const
{
    return glfwGetWindowTitle(m_->window());
}

void GLFWWidget::setTitle(const std::string &value)
{
    glfwSetWindowTitle(m_->window(), value.c_str());
}

void GLFWWidget::setIcon(const std::shared_ptr<utils::Image> &image)
{
    std::shared_ptr<GLFWimage> icon;
    std::vector<uint8_t> pixels;
    
    if (image)
    {
        pixels.resize(image->width() * image->height() * 4u);

        for (uint32_t y = 0u; y < image->height(); ++y)
            for (uint32_t x = 0u; x < image->width(); ++x)
            {
                auto inPixel = image->pixel(x, y);
                std::array<uint8_t, 4u> outPixel = { 0u, 0u, 0u, 255u };

                for (uint32_t c = 0; c < image->numComponents(); ++c)
                {
                    auto component = inPixel + c * utils::sizeOfPixelComponentType(image->type());
                    switch (image->type())
                    {
                    case utils::PixelComponentType::Single:
                    {
                        outPixel[c] = static_cast<uint8_t>(255.f * glm::clamp(*reinterpret_cast<const float*>(component), 0.f, 1.f));
                        break;
                    }
                    case utils::PixelComponentType::Uint8:
                    {
                        outPixel[c] = *reinterpret_cast<const uint8_t*>(component);
                        break;
                    }
                    case utils::PixelComponentType::Uint16:
                    {
                        outPixel[c] = static_cast<uint8_t>(*reinterpret_cast<const uint16_t*>(component) / 256u);
                        break;
                    }
                    default:
                        break;
                    }
                }

                std::memcpy(pixels.data() + 4u * (y * image->width() + x), outPixel.data(), 4u);
            }

        icon = std::make_shared<GLFWimage>();
        icon->width = static_cast<int>(image->width());
        icon->height = static_cast<int>(image->height());
        icon->pixels = pixels.data();
    }

    glfwSetWindowIcon(m_->window(), icon ? 1 : 0, icon.get());
}

glm::uvec2 GLFWWidget::position() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowPos(m_->window(), &x, &y);

    return glm::uvec2(x, y);
}

void GLFWWidget::setPosition(const glm::uvec2 &value)
{
    glfwSetWindowPos(m_->window(), static_cast<int>(value.x), static_cast<int>(value.y));
}

glm::uvec2 GLFWWidget::size() const
{
    int w = 0;
    int h = 0;
    glfwGetWindowSize(m_->window(), &w, &h);

    return glm::uvec2(w, h);
}

void GLFWWidget::setSize(const glm::uvec2& value)
{
    glfwSetWindowSize(m_->window(), static_cast<int>(value.x), static_cast<int>(value.y));
}

void GLFWWidget::minimize()
{
    glfwIconifyWindow(m_->window());
}

void GLFWWidget::maximize()
{
    glfwMaximizeWindow(m_->window());
}

void GLFWWidget::restore()
{
    glfwRestoreWindow(m_->window());
}

void GLFWWidget::show(bool value)
{
    if (!value)
        return hide();

    glfwShowWindow(m_->window());
}

void GLFWWidget::hide()
{
    glfwHideWindow(m_->window());
}

void GLFWWidget::setFocus()
{
    glfwFocusWindow(m_->window());
}

void GLFWWidget::swapBuffers()
{
    glfwSwapBuffers(m_->window());
}

const GLFWWidgetShareGroup& GLFWWidget::shareGroup() const
{
    return *m_->shareGroup();
}

void GLFWWidget::setApplication(const std::shared_ptr<core::ApplicationBase> &value)
{
    m_->application() = value;

    if (auto app = application())
        LOG_INFO << "Application \"" << app->name() << "\" has been set to \"" << name() << "\"";
    else
        LOG_INFO << "Application has been removed from \"" << name() << "\"";
}

std::shared_ptr<core::ApplicationBase> GLFWWidget::application() const
{
    auto &applicationWeak = m_->application();
    return applicationWeak.expired() ? nullptr : applicationWeak.lock();
}

std::shared_ptr<core::graphics::IRenderer> GLFWWidget::renderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::graphics::IRenderer> GLFWWidget::renderer() const
{
    return const_cast<GLFWWidget*>(this)->renderer();
}

std::shared_ptr<core::graphics::IFrameBuffer> GLFWWidget::defaultFrameBuffer()
{
    return m().defaultFrameBuffer();
}

std::shared_ptr<const core::graphics::IFrameBuffer> GLFWWidget::defaultFrameBuffer() const
{
    return const_cast<GLFWWidget*>(this)->defaultFrameBuffer();
}

std::shared_ptr<GLFWWidget> GLFWWidget::getOrCreate(const std::string &name, const std::shared_ptr<GLFWWidget> &sharedWidget)
{
    std::shared_ptr<GLFWWidget> result;

    auto &instances = GLFWWidgetPrivate::instances();
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
        std::shared_ptr<GLFWWidgetShareGroup> shareGroup;
        if (sharedWidget)
            shareGroup = sharedWidget->m().shareGroup();

        if (!shareGroup)
            shareGroup = std::make_shared<GLFWWidgetShareGroup>();

        for (auto it = shareGroup->begin(); it != shareGroup->end(); )
        {
            if (it->expired())
                it = shareGroup->erase(it);
            else
                ++it;
        }

        result = std::shared_ptr<GLFWWidget>(new GLFWWidget(name, shareGroup));
        shareGroup->push_back(result);
        GLFWWidgetPrivate::instances().insert(result);
    }

    return result;
}

void GLFWWidget::run()
{
    while (true)
    {
        bool shouldClose = true;

        for (const auto& widgetWeak : GLFWWidgetPrivate::instances())
        {
            if (widgetWeak.expired())
                continue;

            auto widget = widgetWeak.lock();

            widget->renderer()->makeCurrent();
            glClearColor(.5f, .5f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            widget->swapBuffers();

            shouldClose = false;
        }

        if (shouldClose)
            break;

        glfwPollEvents();
    }

    glfwTerminate();
}

//void QtOpenGLWidget::resizeGL(int width, int height)
//{
//    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
//    {
//        width = pd->width();
//        height = pd->height();
//    }
//
//    LOG_INFO << "QtOpenGLWidget::resizeGL(" << width << ", " << height << ")";
//    if (auto &renderer = m_->renderer(); renderer)
//        renderer->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
//}
//
//void QtOpenGLWidget::paintGL()
//{
//    if (!m_->renderer())
//    {
//        m_->renderer() = QtOpenGL_4_5_Renderer::create(context());
//        m_->defaultFrameBuffer() = std::make_shared<DefaultFrameBuffer_4_5>(defaultFramebufferObject());
//
//        m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
//        m_->lastUpdateTime() = 0u;
//    }
//
//    auto time = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) - m_->startTime();
//    auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
//    m_->lastUpdateTime() = time;
//
//    static const uint64_t deltaFps = 1000u;
//    ++m_->fpsCounter();
//    if (time - m_->lastFpsTime() >= deltaFps)
//    {
//        auto lastFps = static_cast<uint32_t>(static_cast<float>(m_->fpsCounter()) / (0.001f * deltaFps) + 0.5f);
//        m_->fpsCounter() = 0u;
//        m_->lastFpsTime() = time;
//
//        //LOG_INFO << "FPS: " << lastFps;
//    }
//
//    if (auto app = application())
//        app->update(shared_from_this(), time, dt);
//}

GLFWWidget::GLFWWidget(const std::string &name, const std::shared_ptr<GLFWWidgetShareGroup> &shareGroup)
    : m_(std::make_unique<GLFWWidgetPrivate>(name, shareGroup))
{

    if (!GLFWWidgetPrivate::isGLFWInitialized())
    {
        glfwSetErrorCallback(errorCallback);

        if (!glfwInit())
            LOG_CRITICAL << "Failed to initialize GLFW";

        auto tempWindow = glfwCreateWindow(640, 480, name.c_str(), nullptr, nullptr);
        if (!tempWindow)
            LOG_CRITICAL << "Failed to create GLFW window " << name;

        glfwMakeContextCurrent(tempWindow);
        gladLoadGL(glfwGetProcAddress);

        auto version = glad_gl_find_core_gl();
        GLFWWidgetPrivate::majorVersion() = GLAD_VERSION_MAJOR(version);
        GLFWWidgetPrivate::minorVersion() = GLAD_VERSION_MINOR(version);

        glfwDestroyWindow(tempWindow);

        GLFWWidgetPrivate::isGLFWInitialized() = true;
    }

    if (!shareGroup)
        LOG_CRITICAL << "Share group can't be nullptr";

    GLFWwindow* shareWindow = nullptr;
    for (const auto& sharedWidget : *shareGroup)
    {
        if (!sharedWidget.expired())
        {
            shareWindow = sharedWidget.lock()->m().window();
            break;
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFWWidgetPrivate::majorVersion());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFWWidgetPrivate::minorVersion());
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    static const uint32_t width = 640u;
    static const uint32_t height = 480u;

    auto window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, shareWindow);
    if (!window)
        LOG_CRITICAL << "Failed to create GLFW window " << name;

    glfwSetWindowCloseCallback(window, closeCallback);
    glfwSetFramebufferSizeCallback(window, sizeCallback);

    glfwMakeContextCurrent(window);

    auto renderer = GLFWRenderer::getOrCreate(window);
    if (!renderer)
        LOG_CRITICAL << "Failed to create GLFW renderer for window " << name;
    renderer->resize(width, height);

    m_->window() = window;
    m_->renderer() = renderer;
    m_->defaultFrameBuffer() = DefaultFrameBuffer_4_5::create(0u);

    LOG_INFO << "GLFWWidget has been created (Name: \"" << GLFWWidget::name() << 
                "\", OpenGL context ver:" << GLFWWidgetPrivate::majorVersion() << "." <<
                GLFWWidgetPrivate::minorVersion() << ")";
}

}
}
