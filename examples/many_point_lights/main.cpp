#include <utils/boundingbox.h>
#include <utils/transform.h>

#include <core/applicationbase.h>
#include <core/cameranode.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/pointlightnode.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/scenesloader.h>
#include <core/skeletalanimatednode.h>

#include <graphics_glfw/glfwwidget.h>
#include <scenes_loader_assimp/scenesloaderassimp.h>

struct LightNode
{
    std::weak_ptr<simplex::core::Node> node;
    glm::vec3 translation;
    glm::vec3 axis;
    float velocity;
};
static std::vector<LightNode> s_lightNodes;
static std::weak_ptr<simplex::graphics_glfw::GLFWWidget> s_window;
static std::weak_ptr<simplex::core::CameraNode> s_cameraNode;

static std::string s_windowTitle = "Many point lights";

static float randRange(float start, float end)
{
    return glm::mix(start, end, static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    uint32_t lightCount,
    const std::shared_ptr<simplex::core::ScenesLoader>& scenesLoader,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
{
    renderer->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    scene->sceneRootNode()->attach(cameraNode);
    s_cameraNode = cameraNode;

    auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/Sponza/Sponza.gltf");
    auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
    scene->sceneRootNode()->attach(skeletalAnimatedNode);

    const auto& sceneBoundingBox = scene->sceneRootNode()->boundingBox();
    const auto& bbMinPoint = sceneBoundingBox.minPoint();
    const auto& bbMaxPoint = sceneBoundingBox.maxPoint();

    for (uint32_t i = 0u; i < lightCount; ++i)
    {
        auto node = std::make_shared<simplex::core::Node>("");
        scene->sceneRootNode()->attach(node);
        s_lightNodes.push_back(
            {node,
             glm::vec3(
                 randRange(bbMinPoint.x, bbMaxPoint.x), randRange(bbMinPoint.y, bbMaxPoint.y),
                 randRange(bbMinPoint.z, bbMaxPoint.z)),
             glm::normalize(glm::vec3(randRange(-1.f, 1.f), randRange(-1.f, 1.f), randRange(-1.f, 1.f))), randRange(1.f, 2.f)});

        auto pointLightNode = std::make_shared<simplex::core::PointLightNode>("");
        pointLightNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.f, 0.f, randRange(1.f, 2.f))));
        pointLightNode->setColor(glm::vec3(randRange(.3f, 5.f), randRange(.3f, 5.f), randRange(.3f, 5.f)));
        node->attach(pointLightNode);
    }

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
        default:
            break;
    }
}

static void updateCallback(uint64_t time, uint32_t dt)
{
    if (auto cameraNode = s_cameraNode.lock())
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

        const auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(cameraAngles, 0.0f)));

        const auto cameraFowrardDir = cameraRotation.transformVector(glm::vec3(0.0f, 0.0f, -1.0f));
        const auto cameraRightDir = cameraRotation.transformVector(glm::vec3(1.0f, 0.0f, 0.0f));
        const auto cameraUpDir = cameraRotation.transformVector(glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 cameraDir(0.f);
        if (isWPressed) cameraDir += cameraFowrardDir;
        if (isSPressed) cameraDir -= cameraFowrardDir;
        if (isDPressed) cameraDir += cameraRightDir;
        if (isAPressed) cameraDir -= cameraRightDir;
        if (isQPressed) cameraDir -= cameraUpDir;
        if (isEPressed) cameraDir += cameraUpDir;

        const float vel = isLShiftPressed ? .01f : .001f;
        if (glm::length(cameraDir) > .1f) cameraPosition += glm::normalize(cameraDir) * static_cast<float>(dt) * vel;

        const auto cameraTranslation = simplex::utils::Transform::makeTranslation(cameraPosition);

        cameraNode->setTransform(cameraTranslation * cameraRotation);
    }

    for (auto& lightNodeStruct : s_lightNodes)
    {
        if (auto node = lightNodeStruct.node.lock())
        {
            const auto rotation =
                glm::angleAxis(static_cast<float>(time) * .001f * lightNodeStruct.velocity, lightNodeStruct.axis);
            node->setTransform(simplex::utils::Transform(1.f, rotation, lightNodeStruct.translation));
        }
    }

    if (auto window = s_window.lock())
    {
        window->setTitle(s_windowTitle + " FPS = " + std::to_string(simplex::core::ApplicationBase::instance().FPS()));
    }
}

static void closeCallback()
{
    simplex::core::ApplicationBase::instance().setScene(nullptr);
}

int main(int argc, char* argv[])
{
    static constexpr auto LightsCount = 1000u;

    if (!simplex::core::ApplicationBase::initialize(
            []() { return simplex::graphics_glfw::GLFWWidget::time(); },
            []() { simplex::graphics_glfw::GLFWWidget::pollEvents(); }))
    {
        LOG_CRITICAL << "Failed to initialize application";
        return 0;
    }

    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate(s_windowTitle);
    window->setKeyCallback(keyCallback);
    window->setUpdateCallback(updateCallback);
    window->setCloseCallback(closeCallback);
    s_window = window;

    window->graphicsEngine()->scenesLoader()->registerLoader(
        std::make_shared<simplex::scenes_loader_assimp::AssimpScenesLoader>());

    auto& app = simplex::core::ApplicationBase::instance();
    app.setScene(createScene(
        "ManyPointLightsScene", LightsCount, window->graphicsEngine()->scenesLoader(),
        window->graphicsEngine()->graphicsRenderer()));

    app.registerDevice(window);
    app.run();
    app.unregisterDevice(window);

    return 0;
}