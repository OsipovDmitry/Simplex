#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/frustum.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/image.h>
#include <utils/glm/gtc/matrix_transform.hpp>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/pointlightnode.h>
#include <core/spotlightnode.h>
#include <core/directionallightnode.h>
#include <core/ibllightnode.h>
#include <core/drawablenode.h>
#include <core/igraphicsrenderer.h>
#include <core/standarddrawable.h>
#include <core/igraphicsrenderer.h>
#include <core/texturesmanager.h>

#include "testapplication.h"

float random(float a = 0.0f, float b = 1.0f)
{
    auto r = static_cast<float>(rand()) / (RAND_MAX-1.0f);
    return a + r * (b-a);
}

using namespace simplex;

const std::string GraphicsEngineName = "Simplex3DGraphicsEngine";
const std::string SceneName = "Scene0";

TestApplication::TestApplication(std::shared_ptr<simplex::core::graphics::IRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
{   
    auto graphicsEngineInstance = std::make_shared<simplex::core::GraphicsEngine>(GraphicsEngineName, graphicsRenderer);
    registerEngine(graphicsEngineInstance);

    auto texturesManager = graphicsEngineInstance->texturesManager();

    utils::MeshPainter planePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Tangent, {4u, utils::VertexComponentType::Single}}
                                                                }));
    planePainter.setVertexTransform(utils::Transform(10.f, glm::quat(glm::vec3(-glm::half_pi<float>(), 0.f, 0.f))));
    planePainter.setTexCoordsTransform(utils::Transform::fromScale(4.f));
    planePainter.drawPlane();
    auto planeDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(planePainter.mesh()), planePainter.calculateBoundingBox());
    planeDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/bc.jpg"));
    planeDrawable->setNormalMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/n.jpg"));
    planeDrawable->setMetalness(0.0f);
    planeDrawable->setRoughness(0.6f);

    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    teapotPainter.drawTeapot();
    auto teapotBoundingBox = teapotPainter.calculateBoundingBox();
    auto teapotVao = graphicsRenderer->createVertexArray(teapotPainter.mesh());

    std::array<std::array<std::shared_ptr<core::StandardDrawable>, 1u>, 1u> teapotDrawables;
    for (uint32_t r = 0u; r < teapotDrawables.size(); ++r)
        for (uint32_t m = 0u; m < teapotDrawables[r].size(); ++m)
        {
            teapotDrawables[r][m] = std::make_shared<core::StandardDrawable>(teapotVao, teapotBoundingBox);
            teapotDrawables[r][m]->setBaseColor(glm::vec4(1.f, 1.f, 1.f, .5f));
            teapotDrawables[r][m]->setMetalness(static_cast<float>(m) / static_cast<float>(teapotDrawables[r].size()-1u));
            teapotDrawables[r][m]->setRoughness(static_cast<float>(r) / static_cast<float>(teapotDrawables.size()-1u));
        }

    utils::MeshPainter boxPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    boxPainter.drawCube(glm::vec3(0.6f, 1.0f, 0.1f));
    auto boxDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(boxPainter.mesh()), boxPainter.calculateBoundingBox());
    boxDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    boxDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass1.jpg"));

    utils::MeshPainter tetraPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    tetraPainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    tetraPainter.drawTetrahedron();
    auto tetraDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(tetraPainter.mesh()), tetraPainter.calculateBoundingBox());
    tetraDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    tetraDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass2.jpg"));

    utils::MeshPainter spherePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    spherePainter.setVertexTransform(utils::Transform::fromScale(.4f));
    spherePainter.drawSphere(16);
    auto sphereDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(spherePainter.mesh()), spherePainter.calculateBoundingBox());
    sphereDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    sphereDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass3.jpg"));

    utils::MeshPainter suzannePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    suzannePainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    suzannePainter.drawSuzanne();
    auto suzanneDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(suzannePainter.mesh()), suzannePainter.calculateBoundingBox());
    suzanneDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.5f));
    suzanneDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass4.jpg"));

    utils::MeshPainter bunnyPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    bunnyPainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    bunnyPainter.drawBunny();
    auto bunnyDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(bunnyPainter.mesh()), bunnyPainter.calculateBoundingBox());
    bunnyDrawable->setBaseColor(glm::vec4(glm::vec3(255.f/255.f, 223.f/255.f, 40.f/255.f), .5f));
    bunnyDrawable->setMetalness(.5f);
    bunnyDrawable->setRoughness(0.f);

    auto scene0 = graphicsEngineInstance->addEmptyScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(5.0f * glm::vec3(2.5f, 0.5f, 2.5f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/9.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

//    for (uint32_t i = 0; i < 1u; ++i)
//    {
//        auto pointLight = std::make_shared<core::PointLightNode>("");
//        pointLight->setColor(glm::vec3(.2f) + glm::vec3(random(0.f, .8f), random(0.f, 0.8f), random(0.f, .8f)));
//        pointLight->setTransform(utils::Transform::fromTranslation(glm::vec3(random(-5.f, 5.f), 1.75f, random(-5.f, 5.f))));
//        pointLight->setRadiuses(glm::vec2(3.5f, 4.0f));
//        scene0->sceneRootNode()->attach(pointLight);
//    }

    auto dirLight = std::make_shared<core::DirectionalLightNode>("");
    dirLight->setColor(glm::vec3(10.0f));
    dirLight->setTransform(utils::Transform::fromRotation(glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f)));
    scene0->sceneRootNode()->attach(dirLight);

//    auto spotLight = std::make_shared<core::SpotLightNode>("");
//    spotLight->setColor(glm::vec3(8.f, 0.f, 0.f));
//    spotLight->setRadiuses(glm::vec2(2.f, 7.f));
//    spotLight->setHalfAngles(glm::vec2(0.f, glm::pi<float>() / 6));
//    spotLight->setTransform(utils::Transform(1.f, glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f), glm::vec3(2.78139, 2.08128, 2.13792)));
//    scene0->sceneRootNode()->attach(spotLight);

//    auto iblLight = std::make_shared<core::IBLLightNode>("");
//    iblLight->setContribution(1.f);
//    scene0->sceneRootNode()->attach(iblLight);

    auto planeDrawableNode = std::make_shared<core::DrawableNode>("");
    planeDrawableNode->addDrawable(planeDrawable);
    scene0->sceneRootNode()->attach(planeDrawableNode);

    for (uint32_t z = 0u; z < teapotDrawables.size(); ++z)
        for (uint32_t x = 0u; x < teapotDrawables[z].size(); ++x)
        {
            auto teapotDrawableNode = std::make_shared<core::DrawableNode>("");
            teapotDrawableNode->addDrawable(teapotDrawables[z][x]);
            teapotDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(
                1.75f * (.5f + static_cast<float>(x) - static_cast<float>(teapotDrawables[z].size()) * .5f),
                1.75f * (0.f),
                1.75f * (.5f + static_cast<float>(z) - static_cast<float>(teapotDrawables.size()) * .5f))));
            scene0->sceneRootNode()->attach(teapotDrawableNode);
        }

    auto boxDrawableNode = std::make_shared<core::DrawableNode>("");
    boxDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(0.2f, 0.5f, 0.42f)) *
                                  utils::Transform::fromRotation(glm::quat(glm::vec3(-0.4f, 0.3f, 0.0f))));
    boxDrawableNode->addDrawable(boxDrawable);
    scene0->sceneRootNode()->attach(boxDrawableNode);

    auto tetraDrawableNode = std::make_shared<core::DrawableNode>("");
    tetraDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(-0.6f, 0.21f, -1.0f)));
    tetraDrawableNode->addDrawable(tetraDrawable);
    scene0->sceneRootNode()->attach(tetraDrawableNode);

    auto sphereDrawableNode = std::make_shared<core::DrawableNode>("");
    sphereDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(1.4f, 0.4f, -1.f)));
    sphereDrawableNode->addDrawable(sphereDrawable);
    scene0->sceneRootNode()->attach(sphereDrawableNode);

    auto suzanneDrawableNode = std::make_shared<core::DrawableNode>("");
    suzanneDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(0.3f, 0.25f, -1.8f)) *
                                      utils::Transform::fromRotation(glm::quat(glm::vec3(-0.7f, 0.5f, 0.0f))));
    suzanneDrawableNode->addDrawable(suzanneDrawable);
    scene0->sceneRootNode()->attach(suzanneDrawableNode);

    auto bunnyDrawableNode = std::make_shared<core::DrawableNode>("Bunny");
    bunnyDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(1.3f, 0.01f, 1.0f)) *
                                      utils::Transform::fromRotation(glm::quat(glm::vec3(0.0f, -0.5f, 0.0f))));
    bunnyDrawableNode->addDrawable(bunnyDrawable);
    scene0->sceneRootNode()->attach(bunnyDrawableNode);

    //scene0->sceneRootNode()->setTransform(utils::Transform::fromRotation(glm::quat(glm::vec3(0.0f, 1.57f, 0.0f))));

    // temp
//    graphicsEngineInstance->removeScene(scene0);
//    auto testScene = graphicsEngineInstance->loadGLTFSceneFromFile("D:/res/Sponza/Sponza.gltf");
//    testScene->sceneRootNode()->attach(std::make_shared<core::CameraNode>(""));
//    testScene->sceneRootNode()->attach(std::make_shared<core::IBLLightNode>(""));
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return std::dynamic_pointer_cast<core::GraphicsEngine>(engine(GraphicsEngineName));
}

void TestApplication::doUpdate(uint64_t time, uint32_t dt)
{
}
