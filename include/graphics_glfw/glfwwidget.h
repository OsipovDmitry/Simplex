#ifndef GRAPHICS_GLFW_WIDGET_H
#define GRAPHICS_GLFW_WIDGET_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/igraphicswidget.h>

#include <graphics_glfw/forwarddecl.h>
#include <graphics_glfw/glfwglobal.h>

namespace simplex
{
namespace graphics_glfw
{

class GLFWWidgetPrivate;
class GRAPHICS_GLFW_SHARED_EXPORT GLFWWidget : public core::graphics::IGraphicsWidget
{
    NONCOPYBLE(GLFWWidget)
    PRIVATE_IMPL(GLFWWidget)

public:
    ~GLFWWidget() override;

    const std::string &name() const override;

    bool isInitialized() const override;
    
    void update(const std::shared_ptr<core::Scene>&, uint64_t time, uint32_t dt) override;

    std::shared_ptr<core::IEngine> engine() override;
    std::shared_ptr<const core::IEngine> engine() const override;

    std::shared_ptr<core::graphics::ShareGroup> shareGroup() override;
    std::shared_ptr<const core::graphics::ShareGroup> shareGroup() const override;

    std::shared_ptr<core::GraphicsEngine> graphicsEngine() override;
    std::shared_ptr<const core::GraphicsEngine> graphicsEngine() const override;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer() override;
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const override;

    std::string title() const override;
    void setTitle(const std::string&) override;

    void setIcon(const std::shared_ptr<utils::Image>&) override;

    glm::uvec2 position() const override;
    void setPosition(const glm::uvec2&) override;

    glm::uvec2 size() const override;
    void setSize(const glm::uvec2&) override;

    void minimize() override;
    void maximize() override;
    void restore() override;

    void show(bool = true) override;
    void hide() override;

    void setFocus() override;

    glm::ivec2 mouseCursorPosition() const override;
    void setMouseCursorPosition(const glm::ivec2&) override;

    void showMouseCursor(bool = true) override;
    void hideMouseCursor() override;

    void setMouseStandardCursor(core::graphics::MouseStandardCursor) override;
    void setMouseCursor(const std::shared_ptr<utils::Image>&, const glm::uvec2& hotPoint) override;

    bool isMouseCursorInside() const override;

    core::graphics::KeyState keyState(core::graphics::KeyCode) const override;
    std::string keyName(core::graphics::KeyCode) const override;
    core::graphics::MouseButtonState mouseButtonState(core::graphics::MouseButton) const override;

    core::graphics::CloseCallback closeCallback() const override;
    void setCloseCallback(core::graphics::CloseCallback) override;

    core::graphics::ResizeCallback resizeCallback() const override;
    void setResizeCallback(core::graphics::ResizeCallback) override;

    core::graphics::UpdateCallback updateCallback() const override;
    void setUpdateCallback(core::graphics::UpdateCallback) override;

    core::graphics::KeyCallback keyCallback() const override;
    void setKeyCallback(core::graphics::KeyCallback) override;

    core::graphics::MouseCursorMoveCallback mouseCursorMoveCallback() const override;
    void setMouseCursorMoveCallback(core::graphics::MouseCursorMoveCallback) override;

    core::graphics::MouseCursorEnterCallback mouseCursorEnterCallback() const;
    void setMouseCursorEnterCallback(core::graphics::MouseCursorEnterCallback);

    core::graphics::MouseButtonCallback mouseButtonCallback() const override;
    void setMouseButtonCallback(core::graphics::MouseButtonCallback) override;

    core::graphics::MouseScrollCallback mouseScrollCallback() const override;
    void setMouseScrollCallback(core::graphics::MouseScrollCallback) override;

    static std::shared_ptr<GLFWWidget> getOrCreate(const std::string&, const std::shared_ptr<GLFWWidget>& = nullptr);

    static void pollEvents();
    static uint64_t time();

//protected:
//    void resizeGL(int, int) override;
//    void paintGL() override;

private:
    GLFWWidget(const std::string&, const std::shared_ptr<core::graphics::ShareGroup>&);

    std::unique_ptr<GLFWWidgetPrivate> m_;
};

}
}

#endif // GRAPHICS_GLFW_WIDGET_H
