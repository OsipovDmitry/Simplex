#ifndef GRAPHICS_GLFW_WIDGET_PRIVATE_H
#define GRAPHICS_GLFW_WIDGET_PRIVATE_H

#include <core/igraphicswidget.h>

#include <graphics_glfw/forwarddecl.h>

struct GLFWwindow;
struct GLFWcursor;

namespace simplex
{
namespace graphics_glfw
{

class DefaultFrameBuffer_4_5;

class GLFWWidgetPrivate
{
public:
    GLFWWidgetPrivate(const std::string &name, const std::shared_ptr<core::graphics::ShareGroup>&);
    ~GLFWWidgetPrivate();

    std::string &name();
    std::shared_ptr<core::GraphicsEngine>& engine();

    GLFWwindow *&window();
    GLFWcursor *&cursor();
    core::graphics::CloseCallback &closeCallback();
    core::graphics::ResizeCallback &resizeCallback();
    core::graphics::UpdateCallback& updateCallback();
    core::graphics::KeyCallback &keyCallback();
    core::graphics::MouseCursorMoveCallback &mouseCursorMoveCallback();
    core::graphics::MouseCursorEnterCallback &mouseCursorEnterCallback();
    core::graphics::MouseButtonCallback &mouseButtonCallback();
    core::graphics::MouseScrollCallback &mouseScrollCallback();

    std::shared_ptr<core::graphics::ShareGroup> &shareGroup();
    std::shared_ptr<DefaultFrameBuffer_4_5> &defaultFrameBuffer();

    static utils::WeakPtrList<GLFWWidget> &instances();
    static bool& isGLFWInitialized();
    static int &majorVersion();
    static int &minorVersion();

private:
    std::string m_name;
    std::shared_ptr<core::GraphicsEngine> m_engine;

    GLFWwindow *m_GLFWWindow;
    GLFWcursor* m_GLFWCursor;
    core::graphics::CloseCallback m_closeCallback;
    core::graphics::ResizeCallback m_resizeCallback;
    core::graphics::UpdateCallback m_updateCallback;
    core::graphics::KeyCallback m_keyCallback;
    core::graphics::MouseCursorMoveCallback m_mouseCursorMoveCallback;
    core::graphics::MouseCursorEnterCallback m_mouseCursorEnterCallback;
    core::graphics::MouseButtonCallback m_mouseButtonCallback;
    core::graphics::MouseScrollCallback m_mouseScrollCallback;

    std::shared_ptr<core::graphics::ShareGroup> m_shareGroup;
    std::shared_ptr<DefaultFrameBuffer_4_5> m_defaultFrameBuffer;

    static utils::WeakPtrList<GLFWWidget> s_instances;
    static bool s_isGLFWInitialized;
    static int s_majorVersion;
    static int s_minorVersion;
};

}
}

#endif // GRAPHICS_GLFW_WIDGET_PRIVATE_H
