#ifndef GRAPHICS_GLFW_WIDGET_H
#define GRAPHICS_GLFW_WIDGET_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/irenderwidget.h>

#include <graphics_glfw/forwarddecl.h>
#include <graphics_glfw/glfwglobal.h>

namespace simplex
{
namespace graphics_glfw
{

class GLFWWidgetPrivate;
class GRAPHICS_GLFW_SHARED_EXPORT GLFWWidget : public std::enable_shared_from_this<GLFWWidget>,  public core::IRenderWidget
{
    NONCOPYBLE(GLFWWidget)
    PRIVATE_IMPL(GLFWWidget)

public:
    ~GLFWWidget() override;

    const std::string &name() const override;

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

    void swapBuffers() override;

    const GLFWWidgetShareGroup& shareGroup() const;

    void setApplication(const std::shared_ptr<core::ApplicationBase>&);
    std::shared_ptr<core::ApplicationBase> application() const;

    std::shared_ptr<core::graphics::IRenderer> renderer();
    std::shared_ptr<const core::graphics::IRenderer> renderer() const;

    std::shared_ptr<core::graphics::IFrameBuffer> defaultFrameBuffer();
    std::shared_ptr<const core::graphics::IFrameBuffer> defaultFrameBuffer() const;

    static std::shared_ptr<GLFWWidget> getOrCreate(const std::string&, const std::shared_ptr<GLFWWidget>& = nullptr);

    static void run();

//protected:
//    void resizeGL(int, int) override;
//    void paintGL() override;

private:
    GLFWWidget(const std::string&, const std::shared_ptr<GLFWWidgetShareGroup>&);

    std::unique_ptr<GLFWWidgetPrivate> m_;
};

}
}

#endif // GRAPHICS_GLFW_WIDGET_H
