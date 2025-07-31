#include "glfwwidgetprivate.h"

namespace simplex
{
namespace graphics_glfw
{

utils::WeakPtrList<GLFWWidget> GLFWWidgetPrivate::s_instances;
bool GLFWWidgetPrivate::s_isGLFWInitialized = false;
int GLFWWidgetPrivate::s_majorVersion = 0;
int GLFWWidgetPrivate::s_minorVersion = 0;

GLFWWidgetPrivate::GLFWWidgetPrivate(const std::string &name, const std::shared_ptr<core::graphics::ShareGroup> &shareGroup)
    : m_name(name)
    , m_GLFWWindow(nullptr)
    , m_GLFWCursor(nullptr)
    , m_closeCallback(nullptr)
    , m_shareGroup(shareGroup)
{
}

GLFWWidgetPrivate::~GLFWWidgetPrivate() = default;

std::string &GLFWWidgetPrivate::name()
{
    return m_name;
}

std::shared_ptr<core::GraphicsEngine>& GLFWWidgetPrivate::engine()
{
    return m_engine;
}

GLFWwindow *&GLFWWidgetPrivate::window()
{
    return m_GLFWWindow;
}

GLFWcursor *&GLFWWidgetPrivate::cursor()
{
    return m_GLFWCursor;
}

core::graphics::CloseCallback& GLFWWidgetPrivate::closeCallback()
{
    return m_closeCallback;
}

core::graphics::ResizeCallback& GLFWWidgetPrivate::resizeCallback()
{
    return m_resizeCallback;
}

core::graphics::UpdateCallback& GLFWWidgetPrivate::updateCallback()
{
    return m_updateCallback;
}

core::graphics::KeyCallback& GLFWWidgetPrivate::keyCallback()
{
    return m_keyCallback;
}

core::graphics::MouseCursorMoveCallback& GLFWWidgetPrivate::mouseCursorMoveCallback()
{
    return m_mouseCursorMoveCallback;
}

core::graphics::MouseCursorEnterCallback& GLFWWidgetPrivate::mouseCursorEnterCallback()
{
    return m_mouseCursorEnterCallback;
}

core::graphics::MouseButtonCallback& GLFWWidgetPrivate::mouseButtonCallback()
{
    return m_mouseButtonCallback;
}

core::graphics::MouseScrollCallback& GLFWWidgetPrivate::mouseScrollCallback()
{
    return m_mouseScrollCallback;
}

std::shared_ptr<core::graphics::ShareGroup>& GLFWWidgetPrivate::shareGroup()
{
    return m_shareGroup;
}

std::shared_ptr<DefaultFrameBuffer_4_5> &GLFWWidgetPrivate::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

utils::WeakPtrList<GLFWWidget>& GLFWWidgetPrivate::instances()
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
