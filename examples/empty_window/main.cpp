#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>

#include <graphics_glfw/glfwwidget.h>

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::IFrameBuffer> &defaultFramebuffer,
    const std::shared_ptr<simplex::core::graphics::RendererBase> &renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("", defaultFramebuffer);
    scene->sceneRootNode()->attach(cameraNode);

    return scene;
}

int main(int argc, char* argv[])
{
    if (!simplex::core::ApplicationBase::initialize(
        []() { return simplex::graphics_glfw::GLFWWidget::time(); },
        []() { simplex::graphics_glfw::GLFWWidget::pollEvents(); }
    ))
    {
        LOG_CRITICAL << "Failed to initialize application";
        return 0;
    }

    auto& app = simplex::core::ApplicationBase::instance();

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Empty window");
    window->engine()->setScene(createScene("Empty scene", window->defaultFrameBuffer(), window->graphicsEngine()->graphicsRenderer()));

    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}