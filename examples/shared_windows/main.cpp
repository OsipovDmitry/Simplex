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
#include <core/nodecollector.h>

#include <graphics_glfw/glfwwidget.h>

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter teapotPainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::TexCoords, {2u, simplex::utils::VertexComponentType::Single}}
        }));
    teapotPainter.setVertexTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, -.5f, 0.f)));
    teapotPainter.drawBunny();
    auto teapotBoundingBox = teapotPainter.calculateBoundingBox();
    auto teapotVao = renderer->createVertexArray(teapotPainter.mesh());

    auto drawable = std::make_shared<simplex::core::PBRDrawable>(teapotVao, teapotBoundingBox);
    drawable->setBaseColor(glm::convertSRGBToLinear(glm::vec4(1.f, .8f, 0.f, 1.f)));
    drawable->setMetalness(0.f);
    drawable->setRoughness(.2f);

    auto drawableNode = std::make_shared<simplex::core::VisualDrawableNode>("DrawableNode");
    drawableNode->addVisualDrawable(drawable);
    scene->sceneRootNode()->attach(drawableNode);

    auto lighNode = std::make_shared<simplex::core::IBLLightNode>("");
    lighNode->setContribution(.1f);
    scene->sceneRootNode()->attach(lighNode);

    return scene;
}

static void addCameraToScene(
    const std::shared_ptr<simplex::core::Scene> &scene,
    const std::string& cameraName,
    const glm::vec3& cameraPos,
    const std::shared_ptr<simplex::core::graphics::IFrameBuffer>& defaultFramebuffer)
{
    auto cameraNode = std::make_shared<simplex::core::CameraNode>(cameraName, defaultFramebuffer);
    cameraNode->setTransform(simplex::utils::Transform::makeLookAt(
        cameraPos,
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted());
    scene->sceneRootNode()->attach(cameraNode);
}

static const auto updateCallback = [](const std::shared_ptr<simplex::core::Scene>& scene, const std::string& cameraNodeName) {
    auto& nodes = scene->sceneRootNode()->children();

    simplex::core::NodeCollector<simplex::core::CameraNode> cameraNodeCollector;
    scene->sceneRootNode()->acceptDown(cameraNodeCollector);

    for (auto& cameraNode : cameraNodeCollector.nodes())
    {
        cameraNode->setRenderingEnabled(cameraNode->name() == cameraNodeName);
    }
    };

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

    auto window0 = simplex::graphics_glfw::GLFWWidget::getOrCreate("Right View");
    auto scene = createScene(
        "Teapot",
        window0->graphicsEngine()->graphicsRenderer());
    addCameraToScene(scene, "CameraNode0", glm::vec3(2.f, 0.f, 0.f), window0->defaultFrameBuffer());
    window0->setScene(scene);
    //window0->setUpdateCallback([&scene](uint64_t time, uint32_t) { updateCallback(scene, "CameraNode0"); });

    auto window1 = simplex::graphics_glfw::GLFWWidget::getOrCreate("Up View", window0);
    //addCameraToScene(scene, "CameraNode1", glm::vec3(0.f, 2.f, 0.f), window1->defaultFrameBuffer());
    window1->setScene(scene);
    //window1->setUpdateCallback([&scene](uint64_t time, uint32_t) { updateCallback(scene, "CameraNode1"); });

    auto window2 = simplex::graphics_glfw::GLFWWidget::getOrCreate("Forward View", window0);
    //addCameraToScene(scene, "CameraNode2", glm::vec3(0.f, 0.f, 2.f), window2->defaultFrameBuffer());
    window2->setScene(scene);
    //window2->setUpdateCallback([&scene](uint64_t time, uint32_t) { updateCallback(scene, "CameraNode2"); });

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window0);
    app.registerDevice(window1);
    app.registerDevice(window2);
    app.run();
    app.unregisterDevice(window0);
    app.unregisterDevice(window1);
    app.unregisterDevice(window2);

    return 0;
}