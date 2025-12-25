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
#include <core/drawablenode.h>
#include <core/mesh.h>
#include <core/material.h>
#include <core/imagebasedllightnode.h>
#include <core/drawable.h>

#include <graphics_glfw/glfwwidget.h>


static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::RendererBase> &renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter teapotPainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}
        }));
    teapotPainter.setVertexTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, -.5f, 0.f)));
    teapotPainter.drawTeapot();
    auto teapotBoundingBox = teapotPainter.calculateBoundingBox();

    auto mesh = std::make_shared<simplex::core::Mesh>(teapotPainter.mesh(), teapotBoundingBox);

    auto material = std::make_shared<simplex::core::Material>();
    material->setBaseColor(glm::convertSRGBToLinear(glm::vec4(.5f, .5f, 1.f, 1.f)));
    material->setMetalness(0.f);
    material->setRoughness(.2f);

    auto drawable = std::make_shared<simplex::core::Drawable>(mesh, material);

    auto drawableNode = std::make_shared<simplex::core::DrawableNode>("TeapotNode");
    drawableNode->addDrawable(drawable);
    scene->sceneRootNode()->attach(drawableNode);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    cameraNode->setTransform(simplex::utils::Transform::makeLookAt(
        glm::vec3(0.f, 0.f, 2.f),
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted());
    scene->sceneRootNode()->attach(cameraNode);

    auto lightNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    lightNode->setContribution(.5f);
    scene->sceneRootNode()->attach(lightNode);

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
    window->engine()->setScene(createScene("Teapot", window->graphicsEngine()->graphicsRenderer()));
    window->setUpdateCallback([&window](uint64_t time, uint32_t)
        {
            auto& nodes = window->engine()->scene()->sceneRootNode()->children();
            if (auto it = std::find_if(nodes.begin(), nodes.end(), [](const std::shared_ptr<simplex::core::Node>& node) {
                return node->name() == "TeapotNode";}); it != nodes.end())
            {
                (*it)->setTransform(
                    simplex::utils::Transform::makeRotation(glm::qua<float>(glm::vec3(time * 0.001f, time * 0.001f, 0.f))));
            }
        });

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}