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

//tmp
#include <utils/clipspace.h>
#include <utils/frustum.h>
#include <optional>

std::optional<simplex::utils::Frustum> frustum;
std::weak_ptr<simplex::core::DrawableNode> pointDrawableNodeWeak;


static std::shared_ptr<simplex::core::Scene> createScene(
    const std::string& name,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer)
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
    planeDrawableNode->setTransform(simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));
    scene->sceneRootNode()->attach(planeDrawableNode);

    static const size_t NumGeometries = 4u;
    static const std::array<glm::vec3, NumGeometries> nodePositions{
        glm::vec3(-1.f, 0.f, -1.f),
        glm::vec3(-1.f, 0.f, +1.f),
        glm::vec3(+1.f, 1.f, +1.f),
        glm::vec3(+1.f, 0.f, -1.f) };
    static const std::array<simplex::utils::MeshPainter& (simplex::utils::MeshPainter::*)(), NumGeometries> painterFunctions{
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
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}} }));

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

//add frustum and closestPoint to scene
static void addFrustumToScene(
    const std::shared_ptr<simplex::core::Scene>& scene,
    const simplex::utils::Frustum& f)
{
    simplex::utils::MeshPainter frustumPainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}} }));

    frustumPainter.drawFrustum(f.projectionMatrix());
    auto frustumBoundingBox = frustumPainter.calculateBoundingBox();

    auto frustumMesh = std::make_shared<simplex::core::Mesh>(frustumPainter.mesh(), frustumBoundingBox);

    auto frustumMaterial = std::make_shared<simplex::core::Material>();
    frustumMaterial->setBaseColor(glm::vec4(.3f, .3f, 1.f, .3f));
    frustumMaterial->setMetalness(0.f);
    frustumMaterial->setRoughness(.2f);

    auto frustumDrawable = std::make_shared<simplex::core::Drawable>(frustumMesh, frustumMaterial);

    auto frustumDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    frustumDrawableNode->addDrawable(frustumDrawable);
    frustumDrawableNode->setTransform(f.viewTransform().inverted());
    scene->sceneRootNode()->attach(frustumDrawableNode);

    simplex::utils::MeshPainter pointPainter(simplex::utils::Mesh::createEmptyMesh({
        {simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
        {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}} }));

    pointPainter.setVertexTransform(simplex::utils::Transform::makeScale(.07f));
    pointPainter.drawSphere();
    auto pointBoundingBox = pointPainter.calculateBoundingBox();

    auto pointMesh = std::make_shared<simplex::core::Mesh>(pointPainter.mesh(), pointBoundingBox);

    auto pointMaterial = std::make_shared<simplex::core::Material>();
    pointMaterial->setBaseColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    pointMaterial->setMetalness(0.f);
    pointMaterial->setRoughness(.2f);

    auto pointDrawable = std::make_shared<simplex::core::Drawable>(pointMesh, pointMaterial);

    auto pointDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
    pointDrawableNode->addDrawable(pointDrawable);
    scene->sceneRootNode()->attach(pointDrawableNode);
    pointDrawableNodeWeak = pointDrawableNode;
}

static bool isWPressed = false;
static bool isSPressed = false;
static bool isAPressed = false;
static bool isDPressed = false;
static bool isLeftPressed = false;
static bool isRightPressed = false;
static bool isUpPressed = false;
static bool isDownPressed = false;
static glm::vec2 cameraAngles(0.f, 0.f/*-0.37f, 5.5f*/);
static glm::vec3 cameraPosition(-2.94f, 1.83f, 2.44f);

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
    case simplex::core::graphics::KeyCode::Space:
    {
        if (keyState == simplex::core::graphics::KeyState::Pressed)
        {
            if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
                if (auto graphicsEngine = window->graphicsEngine())
                {
                    //graphicsEngine->setF();

                    auto scene = graphicsEngine->scene();
                    simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
                    scene->sceneRootNode()->acceptDown(cameraCollector);
                    if (auto& cameraNode = cameraCollector.nodes().front())
                    {
                        frustum = simplex::utils::Frustum(
                            cameraNode->globalTransform().inverted(),
                            cameraNode->clipSpace(),
                            simplex::utils::Range(.1f, 5.f));

                        addFrustumToScene(scene, frustum.value());
                    }
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

    //tmp
    if (auto pointDrawableNode = pointDrawableNodeWeak.lock(); pointDrawableNode && frustum.has_value())
        pointDrawableNode->setTransform(simplex::utils::Transform::makeTranslation(frustum.value().closestPoint(cameraPosition)));
}

int main(int argc, char* argv[])
{
    auto viewTransform = simplex::utils::Transform();
    auto clipSpace = simplex::utils::ClipSpace::makePerspective(1.f, glm::half_pi<float>());
    auto zRange = simplex::utils::Range(1.f, 10.f);
    simplex::utils::Frustum frustum(viewTransform, clipSpace, zRange);

    glm::vec3 p(0.f, 8.f, -5.f);

    const auto& topPlane = frustum.plane(simplex::utils::Frustum::PlaneIndex::Top);

    auto p1 = p - topPlane.normal() * glm::dot(topPlane.normal(), p - topPlane.anyPoint());

    auto tmp = frustum.projectionMatrix() * glm::vec4(p, 1.f);
    tmp /= tmp.w;
    tmp = glm::vec4(simplex::utils::BoundingBox({ glm::vec3(-1.f), glm::vec3(1.f) }).closestPoint(glm::vec3(tmp)), 1.f);
    auto p2 = glm::inverse(frustum.projectionMatrix()) * tmp;
    p2 /= p2.w;


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