#include <utils/glm/gtc/color_space.hpp>
#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/transform.h>

#include <core/ambientlightnode.h>
#include <core/applicationbase.h>
#include <core/background.h>
#include <core/cameranode.h>
#include <core/directionallightnode.h>
#include <core/drawable.h>
#include <core/drawablenode.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/imagebasedlightnode.h>
#include <core/material.h>
#include <core/mesh.h>
#include <core/nodecollector.h>
#include <core/pointlightnode.h>
#include <core/scene.h>
#include <core/scenerepresentation.h>
#include <core/scenerootnode.h>
#include <core/scenesloader.h>
#include <core/skeletalanimatednode.h>
#include <core/skeletalanimation.h>
#include <core/spotlightnode.h>

#include <graphics_glfw/glfwwidget.h>
#include <scenes_loader_assimp/scenesloaderassimp.h>

std::weak_ptr<simplex::core::ImageBasedLightNode> IBLNodeWeak;
std::weak_ptr<simplex::core::DirectionalLightNode> directionalLightNodeWeak;
std::weak_ptr<simplex::core::DrawableNode> clusterNode;

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    simplex::utils::MeshPainter planePainter(simplex::utils::Mesh::createEmptyMesh(
        {{simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
         {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}},
         {simplex::utils::VertexAttribute::TexCoords, {2u, simplex::utils::VertexComponentType::Single}},
         {simplex::utils::VertexAttribute::Tangent, {4u, simplex::utils::VertexComponentType::Single}}}));
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
    planeMaterial->setRoughness(.3f);
    planeMaterial->setNormalMapScale(.3f);
    planeMaterial->setDoubleSided(false);

    auto planeDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    planeDrawableNode->addDrawable(std::make_shared<simplex::core::Drawable>(planeMesh, planeMaterial));
    planeDrawableNode->setTransform(
        simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));
    scene->sceneRootNode()->attach(planeDrawableNode);

    static const size_t NumGeometries = 4u;
    static const std::array<glm::vec3, NumGeometries> nodePositions{
        glm::vec3(-1.f, 0.f, -1.f), glm::vec3(-1.f, 0.f, +1.f), glm::vec3(+1.f, 1.f, +1.f), glm::vec3(+1.f, 0.f, -1.f)};
    static const std::array<simplex::utils::MeshPainter& (simplex::utils::MeshPainter::*)(), NumGeometries> painterFunctions{
        &simplex::utils::MeshPainter::drawBunny, &simplex::utils::MeshPainter::drawTeapot,
        &simplex::utils::MeshPainter::drawSuzanne, &simplex::utils::MeshPainter::drawTetrahedron};
    static const std::array<glm::vec4, NumGeometries> materialBaseColors{
        glm::convertSRGBToLinear(glm::vec4(.5f, .5f, 1.f, 1.f)), glm::convertSRGBToLinear(glm::vec4(1.f, 0.f, 0.f, .2f)),
        glm::convertSRGBToLinear(glm::vec4(0.f, 1.f, 0.f, 1.f)), glm::convertSRGBToLinear(glm::vec4(0.f, 0.f, 1.f, .3f))};

    for (size_t i = 0u; i < NumGeometries; ++i)
    {
        simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh(
            {{simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
             {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}}));

        (painter.*(painterFunctions[i]))();

        auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh(), painter.calculateBoundingBox());

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
    scene->sceneRootNode()->attach(cameraNode);

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(0.f), glm::vec3(1.f, 0.0f, 0.0f), glm::vec3(0.f, 1.f, 0.f)).inverted());
    directionalLightNode->setColor(glm::vec3(10.f));
    directionalLightNode->setShadingEnabled(true);
    // scene->sceneRootNode()->attach(directionalLightNode);
    directionalLightNodeWeak = directionalLightNode;

    auto pointLightNode = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, 1.8f, 0.f)));
    pointLightNode->setColor(glm::vec3(20.f));
    pointLightNode->setRadiuses(glm::vec2(2.5f, 5.f));
    pointLightNode->setShadingEnabled(true);
    //   scene->sceneRootNode()->attach(pointLightNode);

    auto spotLightNode = std::make_shared<simplex::core::SpotLightNode>("");
    spotLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(2.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)).inverted());
    spotLightNode->setColor(glm::vec3(10.f));
    spotLightNode->setHalfAngles(glm::quarter_pi<float>() * glm::vec2(.25f, 1.f));
    spotLightNode->setRadiuses(glm::vec2(1.f, 10.f));
    spotLightNode->setShadingEnabled(true);
    scene->sceneRootNode()->attach(spotLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(.3f);
    // scene->sceneRootNode()->attach(IBLNode);
    IBLNodeWeak = IBLNode;

    return scene;
}

static std::shared_ptr<simplex::core::Scene> createScene2(
    const std::string& name,
    const std::shared_ptr<simplex::core::ScenesLoader>& scenesLoader,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    scene->sceneRootNode()->attach(cameraNode);

    auto pointLightNode0 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode0->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(6.0f, 3.3f, 0.0f)));
    pointLightNode0->setColor(glm::vec3(3.f, 1.f, 1.f));
    pointLightNode0->setRadiuses(glm::vec2(5.f, 8.f));
    // scene->sceneRootNode()->attach(pointLightNode0);

    auto pointLightNode1 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode1->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.0f, 3.3f, 0.0f)));
    pointLightNode1->setColor(glm::vec3(1.f, 3.f, 1.f));
    pointLightNode1->setRadiuses(glm::vec2(5.f, 8.f));
    // scene->sceneRootNode()->attach(pointLightNode1);

    auto pointLightNode2 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode2->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(-6.0f, 3.3f, 0.0f)));
    pointLightNode2->setColor(glm::vec3(1.f, 1.f, 3.f));
    pointLightNode2->setRadiuses(glm::vec2(5.f, 8.f));
    // scene->sceneRootNode()->attach(pointLightNode2);

    auto spotLightNode = std::make_shared<simplex::core::PointLightNode>("");
    spotLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(1.f, 2.0f, .0f), glm::vec3(0.f, 2.0f, 0.f), glm::vec3(0.f, 1.f, 0.f))
            .inverted());
    spotLightNode->setColor(glm::vec3(5.f));
    spotLightNode->setRadiuses(glm::vec2(15.f, 20.f));
    spotLightNode->setShadowCullPlanesLimits(simplex::utils::Range(1.f, 15.0f));
    spotLightNode->setShadingEnabled(true);
    scene->sceneRootNode()->attach(spotLightNode);

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(0.f, 0.f, .0f), glm::vec3(.35f, -1.0f, .35f), glm::vec3(1.f, 0.f, 0.f))
            .inverted());
    directionalLightNode->setColor(glm::vec3(7.f));
    directionalLightNode->setShadingEnabled(true);
    directionalLightNode->setShadowCascadesCount(4u);
    // scene->sceneRootNode()->attach(directionalLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(0.6f);
    // scene->sceneRootNode()->attach(IBLNode);
    IBLNodeWeak = IBLNode;

    auto ambientLightNode = std::make_shared<simplex::core::AmbientLightNode>("");
    ambientLightNode->setColor(glm::vec3(.07f));
    scene->sceneRootNode()->attach(ambientLightNode);

    // auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/arabic_city/scene.gltf");
    auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/Sponza/Sponza.gltf");
    auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
    scene->sceneRootNode()->attach(skeletalAnimatedNode);

    // auto sceneRepresentation = scenesLoader->loadOrGet("C:/Users/3520136/Downloads/cat_-_walking/scene.gltf");
    // auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
    // scene->sceneRootNode()->attach(skeletalAnimatedNode);

    if (auto skeleton = skeletalAnimatedNode->skeleton())
        if (const auto& anims = skeleton->animations(); !anims.empty())
            skeletalAnimatedNode->setCurrentAnimation(anims.begin()->first);

    return scene;
}

static bool isWPressed = false;
static bool isSPressed = false;
static bool isAPressed = false;
static bool isDPressed = false;
static bool isQPressed = false;
static bool isEPressed = false;
static bool isLeftPressed = false;
static bool isRightPressed = false;
static bool isUpPressed = false;
static bool isDownPressed = false;
static bool isSpacePressed = false;
static bool isLShiftPressed = false;
static glm::vec2 cameraAngles(0.f, -glm::half_pi<float>());
static glm::vec3 cameraPosition(-8.f, 1.f, 0.f);

static void keyCallback(
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
        case simplex::core::graphics::KeyCode::Q:
        {
            isQPressed = keyState != simplex::core::graphics::KeyState::Released;
            break;
        }
        case simplex::core::graphics::KeyCode::E:
        {
            isEPressed = keyState != simplex::core::graphics::KeyState::Released;
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
        case simplex::core::graphics::KeyCode::LeftShift:
        {
            isLShiftPressed = keyState != simplex::core::graphics::KeyState::Released;
            break;
        }
        case simplex::core::graphics::KeyCode::Space:
        {
            if (keyState == simplex::core::graphics::KeyState::Pressed)
            {
                if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
                    if (auto graphicsEngine = window->graphicsEngine())
                    {
                        isSpacePressed = true;
                    }
            }

            break;
        }
        default:
            break;
    }
}

static void updateCallback(uint64_t time, uint32_t dt)
{
    glm::vec2 deltaAngles(0.f, 0.f);

    if (isUpPressed) deltaAngles.x += 1.f;

    if (isDownPressed) deltaAngles.x -= 1.f;

    if (isLeftPressed) deltaAngles.y += 1.f;

    if (isRightPressed) deltaAngles.y -= 1.f;

    if (glm::length(deltaAngles) > .1f) cameraAngles += glm::normalize(deltaAngles) * static_cast<float>(dt) * .001f;

    cameraAngles.x = glm::max(cameraAngles.x, -glm::half_pi<float>());
    cameraAngles.x = glm::min(cameraAngles.x, +glm::half_pi<float>());
    cameraAngles.y = glm::mod(cameraAngles.y, glm::two_pi<float>());

    auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(cameraAngles, 0.0f)));
    auto cameraFowrardDir = cameraRotation.transformVector(glm::vec3(0.0f, 0.0f, -1.0f));
    auto cameraRightDir = cameraRotation.transformVector(glm::vec3(1.0f, 0.0f, 0.0f));
    auto cameraUpDir = cameraRotation.transformVector(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraDir(0.f);

    if (isWPressed) cameraDir += cameraFowrardDir;

    if (isSPressed) cameraDir -= cameraFowrardDir;

    if (isDPressed) cameraDir += cameraRightDir;

    if (isAPressed) cameraDir -= cameraRightDir;

    if (isQPressed) cameraDir -= cameraUpDir;

    if (isEPressed) cameraDir += cameraUpDir;

    const float vel = isLShiftPressed ? .01f : .001f;
    // const float vel = isLShiftPressed ? 3.f : .3f;
    if (glm::length(cameraDir) > .1f) cameraPosition += glm::normalize(cameraDir) * static_cast<float>(dt) * vel;

    auto cameraTranslation = simplex::utils::Transform::makeTranslation(cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    auto envRotation = glm::quat(glm::vec3(0.f, time * 0.001f * 0.02f, 0.0f));

    if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
        if (auto graphicsEngine = window->graphicsEngine())
        {
            auto scene = simplex::core::ApplicationBase::instance().scene();
            simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
            scene->sceneRootNode()->acceptDown(cameraCollector);
            cameraCollector.nodes().front()->setTransform(cameraTransform);

            // scene->background()->setRotation(envRotation);
            // if (auto IBLNode = IBLNodeWeak.lock())
            //     IBLNode->setTransform(simplex::utils::Transform::makeRotation(envRotation).inverted());

            if (isSpacePressed)
            {
                isSpacePressed = false;
            }
        }
}

static void closeCallback()
{
    simplex::core::ApplicationBase::instance().setScene(nullptr);
}

int main(int argc, char* argv[])
{
    if (!simplex::core::ApplicationBase::initialize(
            []() { return simplex::graphics_glfw::GLFWWidget::time(); },
            []() { simplex::graphics_glfw::GLFWWidget::pollEvents(); }))
    {
        LOG_CRITICAL << "Failed to initialize application";
        return 0;
    }

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene");
    window->setKeyCallback(keyCallback);
    window->setUpdateCallback(updateCallback);
    window->setCloseCallback(closeCallback);

    window->graphicsEngine()->scenesLoader()->registerLoader(
        std::make_shared<simplex::scenes_loader_assimp::AssimpScenesLoader>());

    auto& app = simplex::core::ApplicationBase::instance();
    app.setScene(
        createScene2("SimpleScene", window->graphicsEngine()->scenesLoader(), window->graphicsEngine()->graphicsRenderer()));
    // app.setScene(createScene("SimpleScene", window->graphicsEngine()->graphicsRenderer()));

    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}