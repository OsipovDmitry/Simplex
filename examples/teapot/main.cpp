#include <utils/logger.h>
#include <utils/transform.h>
#include <utils/meshpainter.h>
#include <utils/mesh.h>
#include <utils/glm/gtc/color_space.hpp>

#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/visualdrawablenode.h>
#include <core/pbrdrawable.h>
#include <core/ibllightnode.h>

#include <graphics_glfw/glfwwidget.h>

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::IFrameBuffer> &defaultFramebuffer,
    const std::shared_ptr<simplex::core::graphics::RendererBase> &renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter teapotPainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::TexCoords, {2u, simplex::utils::VertexComponentType::Single}}
        }));
    teapotPainter.setVertexTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, -.5f, 0.f)));
    teapotPainter.drawTeapot();
    auto teapotBoundingBox = teapotPainter.calculateBoundingBox();
    auto teapotVao = renderer->createVertexArray(teapotPainter.mesh());

    auto drawable = std::make_shared<simplex::core::PBRDrawable>(teapotVao, teapotBoundingBox);
    drawable->setBaseColor(glm::convertSRGBToLinear(glm::vec4(.5f, .5f, 1.f, 1.0f)));
    drawable->setMetalness(0.f);
    drawable->setRoughness(.2f);

    auto drawableNode = std::make_shared<simplex::core::VisualDrawableNode>("TeapotNode");
    drawableNode->addVisualDrawable(drawable);
    scene->sceneRootNode()->attach(drawableNode);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("", defaultFramebuffer);
    cameraNode->setTransform(simplex::utils::Transform::makeLookAt(
        glm::vec3(0.f, 0.f, 2.f),
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted());
    scene->sceneRootNode()->attach(cameraNode);

    auto lighNode = std::make_shared<simplex::core::IBLLightNode>("");
    lighNode->setContribution(.1f);
    scene->sceneRootNode()->attach(lighNode);

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

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Teapot");
    window->setScene(createScene("Teapot", window->defaultFrameBuffer(), window->graphicsEngine()->graphicsRenderer()));
    window->setUpdateCallback([&window](uint64_t time, uint32_t)
        {
            auto& nodes = window->scene()->sceneRootNode()->children();
            if (auto it = std::find_if(nodes.begin(), nodes.end(), [](const std::shared_ptr<simplex::core::Node>& node) {
                return node->name() == "TeapotNode";}); it != nodes.end())
            {
                (*it)->setTransform(simplex::utils::Transform::makeRotation(glm::qua<float>(glm::vec3(time * 0.001f, time * 0.001f, 0.f))));
            }
        });

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}