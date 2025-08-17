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
#include <core/directionallightnode.h>
#include <core/mesh.h>
#include <core/material.h>
#include <core/shadow.h>

#include <graphics_glfw/glfwwidget.h>


static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::IFrameBuffer> &defaultFramebuffer,
    const std::shared_ptr<simplex::core::graphics::RendererBase> &renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter planePainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::TexCoords, {2u, simplex::utils::VertexComponentType::Single}}
        }));
    planePainter.setVertexTransform(simplex::utils::Transform::makeScale(10.f));
    planePainter.setTexCoordsTransform(simplex::utils::Transform::makeScale(5.f));
    planePainter.drawPlane();
    auto planeBoundingBox = planePainter.calculateBoundingBox();

    auto planeMesh = std::make_shared<simplex::core::Mesh>(planePainter.mesh());

    auto planeMaterial = std::make_shared<simplex::core::Material>();
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::BaseColor,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/bc.jpg"));
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::Normal,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/n.jpg"));
    planeMaterial->setMetalness(0.f);
    planeMaterial->setRoughness(.7f);

    auto planeDrawableNode = std::make_shared<simplex::core::VisualDrawableNode>("");
    planeDrawableNode->addVisualDrawable(
        std::make_shared<simplex::core::PBRDrawable>(planeMesh, planeMaterial, planeBoundingBox));
    scene->sceneRootNode()->attach(planeDrawableNode);
    planeDrawableNode->setTransform(simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));


    static const size_t NumGeometries = 4u;
    static const std::array<glm::vec3, NumGeometries> nodePositions{
        glm::vec3(-1.f, 0.f, -1.f),
        glm::vec3(-1.f, 0.f, +1.f), 
        glm::vec3(+1.f, 1.f, +1.f), 
        glm::vec3(+1.f, 0.f, -1.f)};
    static const std::array<simplex::utils::MeshPainter&(simplex::utils::MeshPainter::*)(), NumGeometries> painterFunctions{
        &simplex::utils::MeshPainter::drawBunny,
        &simplex::utils::MeshPainter::drawTeapot,
        &simplex::utils::MeshPainter::drawSuzanne,
        &simplex::utils::MeshPainter::drawTetrahedron
    };
    static const std::array<glm::vec4, NumGeometries> materialBaseColors{
        glm::convertSRGBToLinear(glm::vec4(.5f, .5f, 1.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(1.f, 0.f, 0.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 1.f, 0.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 0.f, 1.f, 1.f)) };

    for (size_t i = 0u; i < NumGeometries; ++i)
    {
        simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}
            }));
        (painter.*(painterFunctions[i]))();
        auto boundingBox = painter.calculateBoundingBox();

        auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh());

        auto material = std::make_shared<simplex::core::Material>();
        material->setBaseColor(materialBaseColors[i]);
        material->setMetalness(0.f);
        material->setRoughness(.2f);

        auto drawable = std::make_shared<simplex::core::PBRDrawable>(mesh, material, boundingBox);

        auto drawableNode = std::make_shared<simplex::core::VisualDrawableNode>("");
        drawableNode->addVisualDrawable(drawable);
        drawableNode->setTransform(simplex::utils::Transform::makeTranslation(nodePositions[i]));
        scene->sceneRootNode()->attach(drawableNode);
    }

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("", defaultFramebuffer);
    cameraNode->setTransform(simplex::utils::Transform::makeLookAt(
        glm::vec3(-3.f, 2.f, 4.f),
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted());
    scene->sceneRootNode()->attach(cameraNode);

    auto IBLNode = std::make_shared<simplex::core::IBLLightNode>("");
    IBLNode->setContribution(.3f);
    //scene->sceneRootNode()->attach(IBLNode);

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(/*simplex::utils::Transform::makeLookAt(
        glm::vec3(3.f, 2.f, -3.f),
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted()*/
        simplex::utils::Transform::makeTranslation(glm::vec3(10.f, -5.f, 3.f)) * 
        simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));
    directionalLightNode->shadow().setMode(simplex::core::ShadingMode::Opaque);
    directionalLightNode->shadow().setFilter(simplex::core::ShadingFilter::Point);
    scene->sceneRootNode()->attach(directionalLightNode);

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

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene");
    window->engine()->setScene(createScene("SimpleScene", window->defaultFrameBuffer(), window->graphicsEngine()->graphicsRenderer()));

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}