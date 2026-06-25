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
#include <core/dynamicbodynode.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/imagebasedlightnode.h>
#include <core/material.h>
#include <core/mesh.h>
#include <core/nodecollector.h>
#include <core/physicsengine.h>
#include <core/physicsrendererbase.h>
#include <core/pointlightnode.h>
#include <core/scene.h>
#include <core/scenerepresentation.h>
#include <core/scenerootnode.h>
#include <core/scenesloader.h>
#include <core/skeletalanimatednode.h>
#include <core/skeletalanimation.h>
#include <core/spotlightnode.h>
#include <core/staticbodynode.h>

#include <graphics_glfw/glfwwidget.h>
#include <physics_bullet/bulletworld.h>
#include <scenes_loader_assimp/scenesloaderassimp.h>

static bool mouseRotation = false;
static glm::vec2 cameraAngles(-0.4f, 4.7f);
static glm::vec3 cameraPosition(-10.f, 5.3f, -.6f);

static bool toHitBunny = false;
static bool toSwitchDirLight = false;
static bool toSwitchLight1 = false;
static bool toSwitchLight2 = false;
static bool toSwitchLight3 = false;

std::weak_ptr<simplex::core::ImageBasedLightNode> IBLNodeWeak;
std::weak_ptr<simplex::core::DrawableNode> clusterNode;
std::weak_ptr<simplex::core::DirectionalLightNode> directionalLightNodeWeak;
std::weak_ptr<simplex::core::PointLightNode> pointLightNode0Weak;
std::weak_ptr<simplex::core::PointLightNode> pointLightNode1Weak;
std::weak_ptr<simplex::core::PointLightNode> pointLightNode2Weak;
uint32_t directionalLightNodeState = 2u;
uint32_t pointLightNode0State = 0u;
uint32_t pointLightNode1State = 0u;
uint32_t pointLightNode2State = 0u;

static void updateLights()
{
    static const auto updateLight = [](const std::shared_ptr<simplex::core::ShadowedLightNode>& shadowedLightNode, uint32_t state)
    {
        if (state == 0u)
        {
            shadowedLightNode->setLightingEnabled(false);
        }
        else if (state == 1u)
        {
            shadowedLightNode->setLightingEnabled(true);
            shadowedLightNode->setShadingEnabled(true);
            shadowedLightNode->setVolumetricScatteringEnabled(false);
        }
        else if (state == 2u)
        {
            shadowedLightNode->setLightingEnabled(true);
            shadowedLightNode->setShadingEnabled(true);
            shadowedLightNode->setVolumetricScatteringEnabled(true);
        }
    };

    if (auto ln = directionalLightNodeWeak.lock()) updateLight(ln, directionalLightNodeState);
    if (auto ln = pointLightNode0Weak.lock()) updateLight(ln, pointLightNode0State);
    if (auto ln = pointLightNode1Weak.lock()) updateLight(ln, pointLightNode1State);
    if (auto ln = pointLightNode2Weak.lock()) updateLight(ln, pointLightNode2State);
}

class DynamicBodyCustomData : public simplex::core::physics::IBody::CustomData
{
public:
    DynamicBodyCustomData(const std::weak_ptr<simplex::core::DynamicBodyNode>& dynamicBodyNode)
        : m_dynamicBodyNode(dynamicBodyNode)
    {
    }

    std::shared_ptr<simplex::core::DynamicBodyNode> dynamicBodyNode() { return m_dynamicBodyNode.lock(); }

    std::shared_ptr<simplex::core::physics::IDynamicBody> dynamicBody()
    {
        if (auto node = dynamicBodyNode()) return node->dynamicBody();
        return nullptr;
    }

    std::shared_ptr<simplex::core::DrawableNode> drawableNode()
    {
        if (auto node = dynamicBodyNode())
        {
            if (auto& nodeChildren = node->children(); !nodeChildren.empty())
            {
                return nodeChildren[0u]->asDrawableNode();
            }
        }
        return nullptr;
    }

    std::shared_ptr<simplex::core::Material> material()
    {
        if (auto node = drawableNode())
        {
            if (auto& drawables = node->drawables(); !drawables.empty())
            {
                return std::const_pointer_cast<simplex::core::Material>(drawables.begin()->get()->material());
            }
        }
        return nullptr;
    }

private:
    std::weak_ptr<simplex::core::DynamicBodyNode> m_dynamicBodyNode;
};

std::optional<std::reference_wrapper<DynamicBodyCustomData>> selectedBodyCustomData;
glm::vec4 cachedSelectedBodyMaterialBaseColor;
glm::vec3 cachedHitLocalPoint;
std::weak_ptr<simplex::core::DrawableNode> hitSphereDrawableNodeWeak;

static void deselectBody()
{
    if (selectedBodyCustomData.has_value())
    {
        if (auto material = selectedBodyCustomData->get().material()) material->setBaseColor(cachedSelectedBodyMaterialBaseColor);
    }
    selectedBodyCustomData.reset();
}

static void selectBody(
    const std::optional<simplex::core::physics::RendererBase::LineSegmentIntersectionResult>& intersectionResult)
{
    if (!intersectionResult.has_value())
    {
        deselectBody();
        return;
    }

    auto dynamicBody = std::dynamic_pointer_cast<simplex::core::physics::IDynamicBody>(intersectionResult->hitBody);
    if (!dynamicBody)
    {
        deselectBody();
        return;
    }

    cachedHitLocalPoint = intersectionResult->hitPosition;

    if (!dynamicBody->hasCustomData<DynamicBodyCustomData>())
    {
        deselectBody();
        return;
    }

    auto bodyCustomData = std::ref(dynamicBody->customData<DynamicBodyCustomData>());

    if (selectedBodyCustomData.has_value())
    {
        if (selectedBodyCustomData->get().dynamicBodyNode() == bodyCustomData.get().dynamicBodyNode()) return;
    }

    deselectBody();
    cachedSelectedBodyMaterialBaseColor = bodyCustomData.get().material()->baseColor();
    bodyCustomData.get().material()->setBaseColor(glm::vec4(1.f, 0.f, 0.f, .75f));
    selectedBodyCustomData = bodyCustomData;
}

static void hitBody(const glm::vec3& hitDirection)
{
    if (selectedBodyCustomData.has_value())
    {
        selectedBodyCustomData->get().dynamicBody()->applyImpulse(glm::normalize(hitDirection) * 5.f, cachedHitLocalPoint);
    }
}

static void hideHitSphere()
{
    if (auto hitSphereDrawableNode = hitSphereDrawableNodeWeak.lock())
    {
        hitSphereDrawableNode->setTransform(simplex::utils::Transform::makeScale(0.f));
    }
}

static void showHitSphere(const std::shared_ptr<simplex::core::Node>& sceneRootNode, const glm::vec3& cameraPosition)
{
    if (!selectedBodyCustomData.has_value())
    {
        hideHitSphere();
        return;
    }

    auto dynamicBody = selectedBodyCustomData->get().dynamicBody();
    if (!dynamicBody)
    {
        hideHitSphere();
        return;
    }

    if (hitSphereDrawableNodeWeak.expired())
    {
        simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh(
            {{simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}}}));

        painter.drawSphere();

        auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh(), painter.calculateBoundingBox());

        auto material = std::make_shared<simplex::core::Material>();
        material->setBaseColor(glm::vec4(glm::vec3(.5f), 1.f));
        material->setLighted(false);
        material->setShadowCasted(false);

        auto drawable = std::make_shared<simplex::core::Drawable>(mesh, material);

        auto drawableNode = std::make_shared<simplex::core::DrawableNode>("");
        drawableNode->addDrawable(drawable);
        sceneRootNode->attach(drawableNode);
        hitSphereDrawableNodeWeak = drawableNode;
    }

    const auto hitPoint = dynamicBody->transform().transformPoint(cachedHitLocalPoint);
    const auto distToCamera = glm::distance(cameraPosition, hitPoint);
    const auto scale = .01f * distToCamera;

    auto hitSphereDrawableNode = hitSphereDrawableNodeWeak.lock();
    hitSphereDrawableNode->setTransform(simplex::utils::Transform(scale, glm::quat(1.f, glm::vec3(0.f)), hitPoint));
}

static std::shared_ptr<simplex::core::DynamicBodyNode> createBunny()
{
    static std::vector<std::shared_ptr<const simplex::utils::Mesh>> convexHullMeshes;
    if (convexHullMeshes.empty())
    {
        if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
            if (auto graphicsEngine = window->graphicsEngine())
                if (auto scenesLoader = graphicsEngine->scenesLoader())
                {
                    static std::vector<std::filesystem::path> paths{
                        "C:/res/bunny/bunny_phys1.obj", "C:/res/bunny/bunny_phys2.obj", "C:/res/bunny/bunny_phys3.obj",
                        "C:/res/bunny/bunny_phys4.obj"};
                    for (const auto& path : paths)
                    {
                        auto sceneRepresentation = scenesLoader->loadOrGet(path);
                        auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
                        const auto& drawableNode = skeletalAnimatedNode->children().front()->children().front()->asDrawableNode();
                        const auto& drawable = *drawableNode->drawables().begin();
                        convexHullMeshes.push_back(drawable->mesh()->mesh());
                    }
                }
    }

    static std::shared_ptr<simplex::core::Mesh> mesh;
    if (!mesh)
    {
        if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
            if (auto graphicsEngine = window->graphicsEngine())
                if (auto scenesLoader = graphicsEngine->scenesLoader())
                {
                    auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/bunny/bunny.obj");
                    auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
                    const auto& drawableNode = skeletalAnimatedNode->children().front()->children().front()->asDrawableNode();
                    const auto& drawable = *drawableNode->drawables().begin();
                    mesh = std::const_pointer_cast<simplex::core::Mesh>(drawable->mesh());
                }
    }

    auto material = std::make_shared<simplex::core::Material>();
    material->setBaseColor(glm::vec4(
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 1.f));
    material->setMetalness(0.f);
    material->setRoughness(.2f);

    auto drawableNode = std::make_shared<simplex::core::DrawableNode>("");
    drawableNode->addDrawable(std::make_shared<simplex::core::Drawable>(mesh, material));

    auto dynamicBodyNode = std::make_shared<simplex::core::DynamicBodyNode>("");
    auto dynamicBody = dynamicBodyNode->dynamicBody();
    dynamicBody->initializeCustomData<DynamicBodyCustomData>(dynamicBodyNode);
    dynamicBody->setRestitution(.2f);
    dynamicBody->setMass(3.f);

    for (const auto& convexHullMesh : convexHullMeshes)
    {
        std::vector<glm::vec3> vertices;
        if (auto verticesBufferIter = convexHullMesh->vertexBuffers().find(simplex::utils::VertexAttribute::Position);
            verticesBufferIter != convexHullMesh->vertexBuffers().end())
        {
            auto& buffer = verticesBufferIter->second;
            vertices.reserve(buffer->numVertices());
            for (size_t i = 0u; i < buffer->numVertices(); ++i)
            {
                auto* v = reinterpret_cast<const float*>(buffer->vertex(i));
                vertices.push_back(glm::vec3(v[0u], v[1u], v[2u]));
            }
        }
        dynamicBody->addConvexHullShape(vertices);
    }

    dynamicBodyNode->attach(drawableNode);

    return dynamicBodyNode;
}

static std::shared_ptr<simplex::core::DynamicBodyNode> createBox(const glm::vec3& halfSize)
{
    auto dynamicBodyNode = std::make_shared<simplex::core::DynamicBodyNode>("");
    auto dynamicBody = dynamicBodyNode->dynamicBody();
    dynamicBody->initializeCustomData<DynamicBodyCustomData>(dynamicBodyNode);
    dynamicBody->setRestitution(.2f);
    dynamicBody->setMass(1.f);
    dynamicBody->addBoxShape(halfSize);
    dynamicBody->setLinearDamping(.3f);
    dynamicBody->setAngularDamping(.3f);
    dynamicBody->setLinearSleepingThreshold(.05f);
    dynamicBody->setAngularSleepingThreshold(.1f);

    simplex::utils::MeshPainter painter(simplex::utils::Mesh::createEmptyMesh(
        {{simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
         {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}}));
    painter.drawCube(2.f * halfSize);

    auto mesh = std::make_shared<simplex::core::Mesh>(painter.mesh(), painter.calculateBoundingBox());

    auto material = std::make_shared<simplex::core::Material>();
    material->setBaseColor(glm::convertSRGBToLinear(glm::vec4(56.f, 183.f, 255.f, 255.f) / 255.f));
    material->setMetalness(0.f);
    material->setRoughness(.2f);

    auto drawableNode = std::make_shared<simplex::core::DrawableNode>("");
    drawableNode->addDrawable(std::make_shared<simplex::core::Drawable>(mesh, material));

    dynamicBodyNode->attach(drawableNode);

    return dynamicBodyNode;
}

static std::shared_ptr<simplex::core::StaticBodyNode> createConstraintsSceneNode()
{
    std::shared_ptr<simplex::core::StaticBodyNode> result;

    if (auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene"))
        if (auto graphicsEngine = window->graphicsEngine())
            if (auto scenesLoader = graphicsEngine->scenesLoader())
            {
                auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/constraints/constraints.obj");

                result = sceneRepresentation->tmp("");
                result->staticBody()->addPlaneShape(simplex::utils::Plane(glm::vec4(0.f, 1.f, 0.f, 0.f)));

                auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
                const auto& drawableNode = skeletalAnimatedNode->children().front()->children().front()->asDrawableNode();
                const auto& drawable = *drawableNode->drawables().begin();
                std::const_pointer_cast<simplex::core::Material>(drawable->material())
                    ->setBaseColor(glm::convertSRGBToLinear(glm::vec4(235.f, 235.f, 35.f, 255.f) / 255.f));

                result->attach(drawableNode);

                simplex::utils::MeshPainter planePainter(simplex::utils::Mesh::createEmptyMesh(
                    {{simplex::utils::VertexAttribute::Position, {3u, simplex::utils::VertexComponentType::Single}},
                     {simplex::utils::VertexAttribute::Normal, {3u, simplex::utils::VertexComponentType::Single}}}));
                planePainter.drawPlane(glm::vec2(50.f, 20.f));

                auto planeMesh = std::make_shared<simplex::core::Mesh>(planePainter.mesh(), planePainter.calculateBoundingBox());

                auto planeMaterial = std::make_shared<simplex::core::Material>();
                planeMaterial->setBaseColor(glm::convertSRGBToLinear(glm::vec4(35.f, 235.f, 65.f, 255.f) / 255.f));
                planeMaterial->setMetalness(0.f);
                planeMaterial->setRoughness(.3f);

                auto planeDrawableNode = std::make_shared<simplex::core::DrawableNode>("");
                planeDrawableNode->addDrawable(std::make_shared<simplex::core::Drawable>(planeMesh, planeMaterial));
                planeDrawableNode->setTransform(simplex::utils::Transform(
                    1.f, glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f)), glm::vec3(-5.f, 0.f, 0.f)));
                result->attach(planeDrawableNode);
            }

    return result;
}

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
    // scene->sceneRootNode()->attach(spotLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(.5f);
    scene->sceneRootNode()->attach(IBLNode);
    IBLNodeWeak = IBLNode;

    return scene;
}

static std::shared_ptr<simplex::core::Scene> createScene2(
    const std::string& name,
    const std::shared_ptr<simplex::core::ScenesLoader>& scenesLoader,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer,
    const std::shared_ptr<simplex::core::physics::RendererBase>& world)
{
    renderer->makeCurrent();
    world->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    scene->sceneRootNode()->attach(cameraNode);

    auto pointLightNode0 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode0->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(6.0f, 3.3f, 0.0f)));
    pointLightNode0->setColor(glm::vec3(3.f, 1.f, 1.f));
    pointLightNode0->setRadiuses(glm::vec2(5.f, 8.f));
    scene->sceneRootNode()->attach(pointLightNode0);
    pointLightNode0Weak = pointLightNode0;

    auto pointLightNode1 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode1->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(0.0f, 3.3f, 0.0f)));
    pointLightNode1->setColor(glm::vec3(3.f));
    pointLightNode1->setRadiuses(glm::vec2(5.f, 8.f));
    scene->sceneRootNode()->attach(pointLightNode1);
    pointLightNode1Weak = pointLightNode1;

    auto pointLightNode2 = std::make_shared<simplex::core::PointLightNode>("");
    pointLightNode2->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(-6.0f, 3.3f, 0.0f)));
    pointLightNode2->setColor(glm::vec3(1.f, 1.f, 3.f));
    pointLightNode2->setRadiuses(glm::vec2(5.f, 8.f));
    scene->sceneRootNode()->attach(pointLightNode2);
    pointLightNode2Weak = pointLightNode2;

    auto spotLightNode = std::make_shared<simplex::core::PointLightNode>("");
    spotLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(1.f, 2.0f, .0f), glm::vec3(0.f, 2.0f, 0.f), glm::vec3(0.f, 1.f, 0.f))
            .inverted());
    spotLightNode->setColor(glm::vec3(5.f));
    spotLightNode->setRadiuses(glm::vec2(15.f, 20.f));
    spotLightNode->setShadowCullPlanesLimits(simplex::utils::Range(1.f, 15.0f));
    spotLightNode->setShadingEnabled(true);
    // scene->sceneRootNode()->attach(spotLightNode);

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(0.f, 0.f, .0f), glm::vec3(1.0f, -1.0f, .15f), glm::vec3(0.f, 0.f, 1.f))
            .inverted());
    directionalLightNode->setColor(glm::vec3(7.f));
    directionalLightNode->setShadowCascadesCount(4u);
    directionalLightNode->setShadowMapSize(1024u);
    scene->sceneRootNode()->attach(directionalLightNode);
    directionalLightNodeWeak = directionalLightNode;

    updateLights();

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(0.15f);
    scene->sceneRootNode()->attach(IBLNode);
    IBLNodeWeak = IBLNode;

    auto ambientLightNode = std::make_shared<simplex::core::AmbientLightNode>("");
    ambientLightNode->setColor(glm::vec3(.1f));
    // scene->sceneRootNode()->attach(ambientLightNode);

    // auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/arabic_city/scene.gltf");
    auto sceneRepresentation = scenesLoader->loadOrGet("C:/res/Sponza/Sponza.gltf");
    auto skeletalAnimatedNode = sceneRepresentation->generate("", false, false);
    scene->sceneRootNode()->attach(skeletalAnimatedNode);

    if (auto skeleton = skeletalAnimatedNode->skeleton())
        if (const auto& anims = skeleton->animations(); !anims.empty())
            skeletalAnimatedNode->setCurrentAnimation(anims.begin()->first);

    auto sceneRepresentationPhys = scenesLoader->loadOrGet("C:/res/Sponza/sponza_phys.obj");
    auto staticBody = sceneRepresentationPhys->tmp("");
    staticBody->staticBody()->setRestitution(.2f);
    scene->sceneRootNode()->attach(staticBody);

    const std::vector<simplex::utils::Transform> bunnyTransforms{
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 1.57f, 0.f)), glm::vec3(-8.04520035, 4.3, -1.17106831)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 1.57f, 0.f)), glm::vec3(-8.04623699, 4.3, 0.527931511)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, -1.57f, 0.f)), glm::vec3(6.99916792, 4.3, -1.10549867)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, -1.57f, 0.f)), glm::vec3(6.99075460, 4.3, 0.543479741)),
        //
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(-5.79933882, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(-4.12666988, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(-2.860184192, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(-1.20887530, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(0.101911098, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(1.71743131, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(3.09015656, 4.3, 1.66887450)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 3.14f, 0.f)), glm::vec3(4.69883728, 4.3, 1.66887450)),
        //
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(-5.79933882, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(-4.12666988, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(-2.860184192, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(-1.20887530, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(0.101911098, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(1.71743131, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(3.09015656, 4.3, -2.27566624)),
        simplex::utils::Transform(.4f, glm::quat(glm::vec3(0.f, 0.f, 0.f)), glm::vec3(4.69883728, 4.3, -2.27566624)),

    };

    for (const auto& transform : bunnyTransforms)
    {
        auto bunnyNode = createBunny();
        bunnyNode->setTransform(transform);
        // scene->sceneRootNode()->attach(bunnyNode);
    }

    return scene;
}

static std::shared_ptr<simplex::core::Scene> createScene3(
    const std::string& name,
    const std::shared_ptr<simplex::core::ScenesLoader>& scenesLoader,
    const std::shared_ptr<simplex::core::graphics::RendererBase>& renderer,
    const std::shared_ptr<simplex::core::physics::RendererBase>& world)
{
    renderer->makeCurrent();
    world->makeCurrent();

    auto scene = simplex::core::Scene::createEmpty(name);

    auto cameraNode = std::make_shared<simplex::core::CameraNode>("");
    scene->sceneRootNode()->attach(cameraNode);

    auto directionalLightNode = std::make_shared<simplex::core::DirectionalLightNode>("");
    directionalLightNode->setTransform(
        simplex::utils::Transform::makeLookAt(glm::vec3(0.f, 0.f, .0f), glm::vec3(.35f, -1.0f, .35f), glm::vec3(1.f, 0.f, 0.f))
            .inverted());
    directionalLightNode->setColor(glm::vec3(3.f));
    directionalLightNode->setShadingEnabled(true);
    directionalLightNode->setShadowCascadesCount(4u);
    directionalLightNode->setShadowMapSize(2048u);
    scene->sceneRootNode()->attach(directionalLightNode);

    auto IBLNode = std::make_shared<simplex::core::ImageBasedLightNode>("");
    IBLNode->setContribution(0.7f);
    scene->sceneRootNode()->attach(IBLNode);

    auto constraintsStaticBodyNode = createConstraintsSceneNode();
    scene->sceneRootNode()->attach(constraintsStaticBodyNode);

    {
        auto doorNode = createBox(glm::vec3(.37f, .59f, .033f));
        doorNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(-10.026f, 0.589f, 0.f)));
        scene->sceneRootNode()->attach(doorNode);

        auto constraint = doorNode->dynamicBody()->joinHinge(
            simplex::utils::RigidTransform(glm::quat(glm::vec3(glm::half_pi<float>(), 0.f, 0.f)), glm::vec3(.37f, 0.f, 0.f)),
            constraintsStaticBodyNode->staticBody(),
            simplex::utils::RigidTransform(
                glm::quat(glm::vec3(glm::half_pi<float>(), 0.f, 0.f)), glm::vec3(-10.026f + .37f, 0.589f, 0.f)));
        constraint->enableCollisionBetweenLinkedBodies(false);
        constraint->setAngularLimits(simplex::utils::Range(-glm::half_pi<float>() * 1.5f, glm::half_pi<float>() * 1.5f));
    }

    {
        auto slideNode = createBox(glm::vec3(.37f, .59f, .164f));
        slideNode->setTransform(simplex::utils::Transform::makeTranslation(glm::vec3(-16.628f, 0.589f, 0.f)));
        scene->sceneRootNode()->attach(slideNode);

        auto constraint = slideNode->dynamicBody()->joinSlider(
            simplex::utils::RigidTransform(), constraintsStaticBodyNode->staticBody(),
            simplex::utils::RigidTransform::makeTranslation(glm::vec3(-16.628f, 0.589f, 0.f)));
        constraint->enableCollisionBetweenLinkedBodies(false);
        constraint->setLinearLimits(simplex::utils::Range(-2.754f + .37f, 2.754f - .37f));
    }

    static const auto BoxHalfWidth = .065f;
    static const auto BoxHalfLen = .4f;
    static const auto StretchedBoxHalfLen = BoxHalfLen * 1.05f;

    {
        static const auto PivotPoint = glm::vec3(4.971f, 1.93f, 0.f);

        auto boxNode0 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode0->setTransform(simplex::utils::Transform::makeTranslation(PivotPoint - glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode0->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode0);

        auto constraint0 = boxNode0->dynamicBody()->joinPointToPoint(
            glm::vec3(0.f, StretchedBoxHalfLen, 0.f), constraintsStaticBodyNode->staticBody(), PivotPoint);
        constraint0->enableCollisionBetweenLinkedBodies(false);

        auto boxNode1 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode1->setTransform(
            simplex::utils::Transform::makeTranslation(PivotPoint - 1.5f * glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode1->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode1);

        auto constraint1 = boxNode1->dynamicBody()->joinPointToPoint(
            glm::vec3(0.f, StretchedBoxHalfLen, 0.f), boxNode0->dynamicBody(), glm::vec3(0.f, -StretchedBoxHalfLen, 0.f));
        constraint1->enableCollisionBetweenLinkedBodies(false);
    }

    {
        static const auto PivotPoint = glm::vec3(0.f, 1.93f, 0.f);

        auto boxNode0 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode0->setTransform(simplex::utils::Transform::makeTranslation(PivotPoint - glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode0->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode0);

        auto constraint0 = boxNode0->dynamicBody()->joinPointToPoint(
            glm::vec3(0.f, StretchedBoxHalfLen, 0.f), constraintsStaticBodyNode->staticBody(), PivotPoint);
        constraint0->enableCollisionBetweenLinkedBodies(false);

        auto boxNode1 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode1->setTransform(
            simplex::utils::Transform::makeTranslation(PivotPoint - 1.5f * glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode1->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode1);

        auto constraint1 = boxNode1->dynamicBody()->joinFixed(
            simplex::utils::RigidTransform::makeTranslation(glm::vec3(0.f, StretchedBoxHalfLen, 0.f)), boxNode0->dynamicBody(),
            simplex::utils::RigidTransform::makeTranslation(glm::vec3(0.f, -StretchedBoxHalfLen, 0.f)));
        constraint1->enableCollisionBetweenLinkedBodies(false);
    }

    {
        static const auto PivotPoint = glm::vec3(-4.983f, 1.93f, 0.f);

        auto boxNode0 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode0->setTransform(simplex::utils::Transform::makeTranslation(PivotPoint - glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode0->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode0);

        auto constraint0 = boxNode0->dynamicBody()->joinHinge(
            simplex::utils::RigidTransform(
                glm::quat(glm::vec3(0.f, glm::half_pi<float>(), 0.f)), glm::vec3(0.f, StretchedBoxHalfLen, 0.f)),
            constraintsStaticBodyNode->staticBody(),
            simplex::utils::RigidTransform(glm::quat(glm::vec3(0.f, glm::half_pi<float>(), 0.f)), PivotPoint));
        constraint0->enableCollisionBetweenLinkedBodies(false);

        auto boxNode1 = createBox(glm::vec3(BoxHalfWidth, BoxHalfLen, BoxHalfWidth));
        boxNode1->setTransform(
            simplex::utils::Transform::makeTranslation(PivotPoint - 1.5f * glm::vec3(0.f, StretchedBoxHalfLen, 0.f)));
        boxNode1->dynamicBody()->setAngularDamping(.9f);
        scene->sceneRootNode()->attach(boxNode1);

        auto constraint1 = boxNode1->dynamicBody()->joinHinge(
            simplex::utils::RigidTransform(
                glm::quat(glm::vec3(0.f, glm::half_pi<float>(), 0.f)), glm::vec3(0.f, StretchedBoxHalfLen, 0.f)),
            boxNode0->dynamicBody(),
            simplex::utils::RigidTransform(
                glm::quat(glm::vec3(0.f, glm::half_pi<float>(), 0.f)), glm::vec3(0.f, -StretchedBoxHalfLen, 0.f)));
        constraint1->enableCollisionBetweenLinkedBodies(false);
    }

    return scene;
}

static void keyCallback(
    simplex::core::graphics::KeyState keyState,
    simplex::core::graphics::KeyCode keyCode,
    const simplex::core::graphics::KeyModifiers&)
{
    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene");
    assert(window != nullptr);

    switch (keyCode)
    {
        case simplex::core::graphics::KeyCode::Space:
        {
            if (keyState == simplex::core::graphics::KeyState::Pressed)
            {
                mouseRotation = !mouseRotation;
                if (mouseRotation)
                {
                    std::array<uint8_t, 4u> data{0u, 0u, 0u, 0u};
                    window->setMouseCursor(
                        std::make_shared<simplex::utils::Image>(
                            1u, 1u, 4u, simplex::utils::PixelComponentType::Uint8, data.data()),
                        glm::uvec2(0));
                    window->setMouseCursorPosition(window->size() / 2u);
                }
                else
                {
                    window->setMouseStandardCursor(simplex::core::graphics::MouseStandardCursor::Arrow);
                }
            }
            break;
        }
        case simplex::core::graphics::KeyCode::Key_0:
        {
            toSwitchDirLight = keyState == simplex::core::graphics::KeyState::Pressed;
            break;
        }
        case simplex::core::graphics::KeyCode::Key_1:
        {
            toSwitchLight1 = keyState == simplex::core::graphics::KeyState::Pressed;
            break;
        }
        case simplex::core::graphics::KeyCode::Key_2:
        {
            toSwitchLight2 = keyState == simplex::core::graphics::KeyState::Pressed;
            break;
        }
        case simplex::core::graphics::KeyCode::Key_3:
        {
            toSwitchLight3 = keyState == simplex::core::graphics::KeyState::Pressed;
            break;
        }
        default:
            break;
    }
}

static void mouseButtonCallback(
    simplex::core::graphics::MouseButtonState mouseButtonState,
    simplex::core::graphics::MouseButton mouseButton,
    const simplex::core::graphics::KeyModifiers&)
{
    switch (mouseButton)
    {
        case simplex::core::graphics::MouseButton::Left:
        {
            toHitBunny = mouseButtonState == simplex::core::graphics::MouseButtonState::Pressed;
            break;
        }
        default:
            break;
    }
}

static void updateCallback(uint64_t time, uint32_t dt)
{
    auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Simple scene");
    assert(window != nullptr);

    glm::vec2 deltaAngles(0.f, 0.f);

    if (window->keyState(simplex::core::graphics::KeyCode::Up) == simplex::core::graphics::KeyState::Pressed)
        deltaAngles.x += 10.f;

    if (window->keyState(simplex::core::graphics::KeyCode::Down) == simplex::core::graphics::KeyState::Pressed)
        deltaAngles.x -= 10.f;

    if (window->keyState(simplex::core::graphics::KeyCode::Left) == simplex::core::graphics::KeyState::Pressed)
        deltaAngles.y += 10.f;

    if (window->keyState(simplex::core::graphics::KeyCode::Right) == simplex::core::graphics::KeyState::Pressed)
        deltaAngles.y -= 10.f;

    if (mouseRotation)
    {
        const auto center = glm::ivec2(window->size() / 2u);
        const auto delta = glm::vec2(window->mouseCursorPosition() - center);
        window->setMouseCursorPosition(center);

        deltaAngles.x -= delta.y;
        deltaAngles.y -= delta.x;
    }

    cameraAngles += deltaAngles * static_cast<float>(dt) * .0001f;

    cameraAngles.x = glm::clamp(cameraAngles.x, -glm::half_pi<float>(), glm::half_pi<float>());
    cameraAngles.y = glm::mod(cameraAngles.y, glm::two_pi<float>());

    auto cameraRotation = simplex::utils::Transform::makeRotation(glm::quat(glm::vec3(cameraAngles, 0.0f)));
    auto cameraFowrardDir = cameraRotation.transformVector(glm::vec3(0.0f, 0.0f, -1.0f));
    auto cameraRightDir = cameraRotation.transformVector(glm::vec3(1.0f, 0.0f, 0.0f));
    auto cameraUpDir = cameraRotation.transformVector(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraDir(0.f);

    if (window->keyState(simplex::core::graphics::KeyCode::W) == simplex::core::graphics::KeyState::Pressed)
        cameraDir += cameraFowrardDir;

    if (window->keyState(simplex::core::graphics::KeyCode::S) == simplex::core::graphics::KeyState::Pressed)
        cameraDir -= cameraFowrardDir;

    if (window->keyState(simplex::core::graphics::KeyCode::D) == simplex::core::graphics::KeyState::Pressed)
        cameraDir += cameraRightDir;

    if (window->keyState(simplex::core::graphics::KeyCode::A) == simplex::core::graphics::KeyState::Pressed)
        cameraDir -= cameraRightDir;

    if (window->keyState(simplex::core::graphics::KeyCode::Q) == simplex::core::graphics::KeyState::Pressed)
        cameraDir -= cameraUpDir;

    if (window->keyState(simplex::core::graphics::KeyCode::E) == simplex::core::graphics::KeyState::Pressed)
        cameraDir += cameraUpDir;

    const float vel =
        (window->keyState(simplex::core::graphics::KeyCode::LeftShift) == simplex::core::graphics::KeyState::Pressed) ? .01f
                                                                                                                      : .0015f;
    if (glm::length(cameraDir) > .1f) cameraPosition += glm::normalize(cameraDir) * static_cast<float>(dt) * vel;

    auto cameraTranslation = simplex::utils::Transform::makeTranslation(cameraPosition);
    auto cameraTransform = cameraTranslation * cameraRotation;

    auto scene = simplex::core::ApplicationBase::instance().scene();
    simplex::core::NodeCollector<simplex::core::CameraNode> cameraCollector;
    scene->sceneRootNode()->acceptDown(cameraCollector);
    cameraCollector.nodes().front()->setTransform(cameraTransform);

    if (toHitBunny)
    {
        toHitBunny = false;
        hitBody(cameraFowrardDir);
    }

    if (toSwitchDirLight)
    {
        toSwitchDirLight = false;
        directionalLightNodeState = (directionalLightNodeState + 1u) % 3u;
        updateLights();
    }

    if (toSwitchLight1)
    {
        toSwitchLight1 = false;
        pointLightNode0State = (pointLightNode0State + 1u) % 3u;
        updateLights();
    }

    if (toSwitchLight2)
    {
        toSwitchLight2 = false;
        pointLightNode1State = (pointLightNode1State + 1u) % 3u;
        updateLights();
    }

    if (toSwitchLight3)
    {
        toSwitchLight3 = false;
        pointLightNode2State = (pointLightNode2State + 1u) % 3u;
        updateLights();
    }

    const simplex::utils::LineSegment ls({cameraPosition, cameraPosition + cameraFowrardDir * 5.f});
    const auto res = simplex::core::physics::RendererBase::current()->singleIntersection(ls);
    selectBody(res);
    showHitSphere(simplex::core::ApplicationBase::instance().scene()->sceneRootNode(), cameraPosition);
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
    window->setMouseButtonCallback(mouseButtonCallback);
    window->setUpdateCallback(updateCallback);

    window->graphicsEngine()->scenesLoader()->registerLoader(
        std::make_shared<simplex::scenes_loader_assimp::AssimpScenesLoader>());

    auto world = simplex::physics_bullet::BulletWorld::getOrCreate("Simple scene");

    auto& app = simplex::core::ApplicationBase::instance();
    // app.setScene(createScene("SimpleScene", window->graphicsEngine()->graphicsRenderer()));
    // app.setScene(
    //    createScene2("SimpleScene", window->graphicsEngine()->scenesLoader(), window->graphicsEngine()->graphicsRenderer()));
    app.setScene(createScene2(
        "SimpleScene", window->graphicsEngine()->scenesLoader(), window->graphicsEngine()->graphicsRenderer(),
        world->physicsEngine()->physicsRenderer()));

    app.registerDevice(window);
    app.registerDevice(world);
    app.run();
    app.unregisterDevice(world);
    app.unregisterDevice(window);

    return 0;
}