#include "glfwwidgetprivate.h"

namespace simplex
{
namespace graphics_glfw
{

std::unordered_set<std::weak_ptr<GLFWWidget>, utils::WeakPtrHash<GLFWWidget>> GLFWWidgetPrivate::s_instances;
bool GLFWWidgetPrivate::s_isGLFWInitialized = false;
int GLFWWidgetPrivate::s_majorVersion = 0;
int GLFWWidgetPrivate::s_minorVersion = 0;

GLFWWidgetPrivate::GLFWWidgetPrivate(const std::string &name, const std::shared_ptr<GLFWWidgetShareGroup> &shareGroup)
    : m_name(name)
    , m_shareGroup(shareGroup)
    , m_GLFWWindow(nullptr)
{
}

GLFWWidgetPrivate::~GLFWWidgetPrivate() = default;

std::string &GLFWWidgetPrivate::name()
{
    return m_name;
}

GLFWwindow *&GLFWWidgetPrivate::window()
{
    return m_GLFWWindow;
}

std::shared_ptr<GLFWWidgetShareGroup>& GLFWWidgetPrivate::shareGroup()
{
    return m_shareGroup;
}

std::weak_ptr<core::ApplicationBase> &GLFWWidgetPrivate::application()
{
    return m_application;
}

std::shared_ptr<GLFWRenderer> &GLFWWidgetPrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<DefaultFrameBuffer_4_5> &GLFWWidgetPrivate::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

uint64_t &GLFWWidgetPrivate::startTime()
{
    return m_startTime;
}

uint64_t &GLFWWidgetPrivate::lastUpdateTime()
{
    return m_lastUpdateTime;
}

uint64_t &GLFWWidgetPrivate::lastFpsTime()
{
    return m_lastFpsTime;
}

uint32_t &GLFWWidgetPrivate::fpsCounter()
{
    return m_fpsCounter;
}

std::unordered_set<std::weak_ptr<GLFWWidget>, utils::WeakPtrHash<GLFWWidget>> & GLFWWidgetPrivate::instances()
{
    return s_instances;
}

bool& GLFWWidgetPrivate::isGLFWInitialized()
{
    return s_isGLFWInitialized;
}

int& GLFWWidgetPrivate::majorVersion()
{
    return s_majorVersion;
}

int& GLFWWidgetPrivate::minorVersion()
{
    return s_minorVersion;
}

}
}
