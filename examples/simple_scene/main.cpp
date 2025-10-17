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
#include <core/drawable.h>
#include <core/imagebasedllightnode.h>
#include <core/directionallightnode.h>
#include <core/pointlightnode.h>
#include <core/mesh.h>
#include <core/material.h>
#include <core/shadow.h>
#include <core/nodecollector.h>

#include <graphics_glfw/glfwwidget.h>


static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
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

    auto planeMesh = std::make_shared<simplex::core::Mesh>(planePainter.mesh(), planeBoundingBox);

    auto planeMaterial = std::make_shared<simplex::core::Material>();
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::BaseColor,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/bc.jpg"));
    planeMaterial->setMaterialMap(
        simplex::core::MaterialMapTarget::Normal,
        std::make_shared<simplex::core::MaterialMap>("./resources/textures/brick/n.jpg"));
    planeMaterial->setMetalness(0.f);
    planeMaterial->setRoughness(.7f);

    auto planeDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    planeDrawableNode->addDrawable(
        std::make_shared<simplex::core::Drawable>(planeMesh, planeMaterial));
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
        glm::convertSRGBToLinear(glm::vec4(1.f, 0.f, 0.f, .5f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 1.f, 0.f, 1.f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 0.f, 1.f, 1.f)) };

    for (size_t i = 0u; i < NumGeometries; ++i)
    {
        simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}}));

        (painter.*(painterFunctions[i]))();
        auto boundingBox = painter.calculateBoundingBox();

        auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh(), boundingBox);

        auto material = std::make_shared<simplex::core::Material>();
        material->setBaseColor(materialBaseColors[i]);
        material->setMetalness(0.f);
        material->setRoughness(.2f);

        auto drawable = std::make_shared<simplex::core::Drawable>(mesh, material);

        auto drawableNode = std::make_shared<simplex::core::DrawableNode>("");
        drawableNode->addDrawable(drawable);
        drawableNode->setTransform(simplex::utils::Transform::makeTranslation(nodePositions[i]));
        scene->sceneRootNode()->attach(drawableNode);
    }

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    cameraNode->setTransform(simplex::utils::Transform::makeLookAt(
        glm::vec3(-3.f, 2.f, 4.f),
        glm::vec3(0.f),
        glm::vec3(0.f, 1.f, 0.f)).inverted());
    scene->sceneRootNode()->attach(cameraNode);

    auto pointLightNode = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, 3.f, 0.f)));
    pointLightNode->setColor(glm::vec3(3.f));
    pointLightNode->setRadiuses(glm::vec2(5.f, 8.f));
    scene->sceneRootNode()->attach(pointLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
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

static bool isWPressed = false;
static bool isSPressed = false;
static bool isAPressed = false;
static bool isDPressed = false;
static bool isLeftPressed = false;
static bool isRightPressed = false;
static bool isUpPressed = false;
static bool isDownPressed = false;
static glm::vec2 cameraAngles(-0.37f, 5.5f);
static glm::vec3 cameraPosition(-2.94f, 1.83f, 2.44f);

void keyCallback(
    simplex::core::graphics::KeyState keyState,
    simplex::core::graphics::KeyCode keyCode,
    const simplex::core::graphics::KeyModifiers&)
{
    switch (keyCode)
    {
    case simplex::core::graphics::KeyCode::W:
    {
        isWPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::S:
    {
        isSPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::A:
    {
        isAPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::D:
    {
        isDPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Left:
    {
        isLeftPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Right:
    {
        isRightPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Up:
    {
        isUpPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    case simplex::core::graphics::KeyCode::Down:
    {
        isDownPressed = keyState != simplex::core::graphics::KeyState::Released;
        break;
    }
    default:
        break;
    }
}

void updateCallback(uint64_t time, uint32_t dt)
{
    glm::vec2 deltaAngles(0.f, 0.f);

    if (isUpPressed)
        deltaAngles.x += 1.f;

    if (isDownPressed)
        deltaAngles.x -= 1.f;

    if (isLeftPressed)
        deltaAngles.y += 1.f;

    if (isRightPressed)
        deltaAngles.y -= 1.f;

    if (glm::length(deltaAngles) > .1f)
        cameraAngles += glm::normalize(deltaAngles) * static_cast<float>(dt) * .001f;

    cameraAngles.x = glm::max(cameraAngles.x, -glm::half_pi<float>());
    cameraAngles.x = glm::min(cameraAngles.x, +glm::half_pi<float>());
    cameraAngles.y = glm::mod(cameraAngles.y, glm::two_pi<float>());

    auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(cameraAngles, 0.0f)));
    auto cameraFowrardDir = cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f);
    auto cameraRightDir = cameraRotation * glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraDir(0.f);

    if (isWPressed)
        cameraDir += cameraFowrardDir;

    if (isSPressed)
        cameraDir -= cameraFowrardDir;

    if (isDPressed)
        cameraDir += cameraRightDir;

    if (isAPressed)
        cameraDir -= cameraRightDir;

    if (glm::length(cameraDir) > .1f)
        cameraPosition += glm::normalize(cameraDir) * static_cast<float>(dt) * .003f;

    auto cameraTranslation = simplex::utils::Transform::makeTranslation(cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
        if (auto graphicsEngine = window->graphicsEngine())
        {
            auto scene = graphicsEngine->scene();
            simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
            scene->sceneRootNode()->acceptDown(cameraCollector);
            cameraCollector.nodes().front()->setTransform(cameraTransform);
        }
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
    window->engine()->setScene(createScene("SimpleScene", window->graphicsEngine()->graphicsRenderer()));
    window->setKeyCallback(keyCallback);
    window->setUpdateCallback(updateCallback);

    auto& app = simplex::core::ApplicationBase::instance();
    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}