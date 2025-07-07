#ifndef GRAPHICS_GLFW_WIDGET_PRIVATE_H
#define GRAPHICS_GLFW_WIDGET_PRIVATE_H

#include <memory>
#include <string>

#include <utils/weakptrhash.h>

#include <graphics_glfw/glfwwidget.h>

struct GLFWwindow;

namespace simplex
{
namespace graphics_glfw
{

using utils::operator ==;

class DefaultFrameBuffer_4_5;
class GLFWRenderer;

class GLFWWidgetPrivate
{
public:
    GLFWWidgetPrivate(const std::string &name, const std::shared_ptr<GLFWWidgetShareGroup>&);
    ~GLFWWidgetPrivate();

    std::string &name();
    GLFWwindow *&window();
    std::shared_ptr<GLFWWidgetShareGroup> &shareGroup();

    std::weak_ptr<core::ApplicationBase> &application();
    std::shared_ptr<GLFWRenderer> &renderer();
    std::shared_ptr<DefaultFrameBuffer_4_5> &defaultFrameBuffer();

    uint64_t &startTime();
    uint64_t &lastUpdateTime();
    uint64_t &lastFpsTime();
    uint32_t &fpsCounter();

    static std::unordered_set<std::weak_ptr<GLFWWidget>, utils::WeakPtrHash<GLFWWidget>> &instances();
    static bool& isGLFWInitialized();
    static int &majorVersion();
    static int &minorVersion();

private:
    std::string m_name;
    GLFWwindow *m_GLFWWindow;
    std::shared_ptr<GLFWWidgetShareGroup> m_shareGroup;
    std::weak_ptr<core::ApplicationBase> m_application;
    std::shared_ptr<GLFWRenderer> m_renderer;
    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;
    uint64_t m_startTime;
    uint64_t m_lastUpdateTime;
    uint64_t m_lastFpsTime;
    uint32_t m_fpsCounter;

    static std::unordered_set<std::weak_ptr<GLFWWidget>, utils::WeakPtrHash<GLFWWidget>> s_instances;
    static bool s_isGLFWInitialized;
    static int s_majorVersion;
    static int s_minorVersion;
};

}
}

#endif // GRAPHICS_GLFW_WIDGET_PRIVATE_H
