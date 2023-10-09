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
    auto planeDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(planePainter.drawPlane().mesh()));
    planeDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/bc.jpg"));
    planeDrawable->setNormalMap(texturesManager->loadOrGetTexture("D:/res/textures/brick/n.jpg"));
    planeDrawable->setMetalness(0.0f);
    planeDrawable->setRoughness(0.2f);

    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    auto teapotDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(teapotPainter.drawTeapot().mesh()));
    //teapotDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass0.jpg"));
    teapotDrawable->setMetalness(0.0f);
    teapotDrawable->setRoughness(1.0f);

    utils::MeshPainter boxPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    auto boxDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(boxPainter.drawCube(glm::vec3(0.6f, 1.0f, 0.1f)).mesh()));
    //boxDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.3f));
    boxDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass1.jpg"));

    utils::MeshPainter tetraPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    tetraPainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    auto tetraDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(tetraPainter.drawTetrahedron().mesh()));
    //tetraDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.3f));
    tetraDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass2.jpg"));

    utils::MeshPainter spherePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    spherePainter.setVertexTransform(utils::Transform::fromScale(0.4f));
    auto sphereDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(spherePainter.drawSphere(16).mesh()));
    //sphereDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.4f));
    sphereDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass3.jpg"));

    utils::MeshPainter suzannePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                   {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    suzannePainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    auto suzanneDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(suzannePainter.drawSuzanne().mesh()));
    //suzanneDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.4f));
    suzanneDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass4.jpg"));

    utils::MeshPainter bunnyPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    bunnyPainter.setVertexTransform(utils::Transform::fromScale(0.8f));
    auto bunnyDrawable = std::make_shared<core::StandardDrawable>(graphicsRenderer->createVertexArray(bunnyPainter.drawBunny().mesh()));
    //bunnyDrawable->setBaseColor(glm::vec4(glm::vec3(1.f), 0.4f));
    bunnyDrawable->setBaseColorMap(texturesManager->loadOrGetTexture("D:/res/textures/glass2.jpg"));

    auto scene0 = graphicsEngineInstance->addNewScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(5.0f * glm::vec3(2.5f, 0.5f, 2.5f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/9.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

//    auto pointLight = std::make_shared<core::PointLightNode>("");
//    pointLight->setRadiuses(glm::vec2(1.f, 3.f));
//    pointLight->setTransform(utils::Transform::fromTranslation(glm::vec3(-2.78139, 1.08128, 2.13792)));
//    scene0->sceneRootNode()->attach(pointLight);

//    auto dirLight = std::make_shared<core::DirectionalLightNode>("");
//    dirLight->setTransform(utils::Transform::fromRotation(glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f)));
//    scene0->sceneRootNode()->attach(dirLight);

//    auto spotLight = std::make_shared<core::SpotLightNode>("");
//    spotLight->setRadiuses(glm::vec2(2.f, 7.f));
//    spotLight->setHalfAngles(glm::vec2(glm::pi<float>() / 8, glm::pi<float>() / 6));
//    spotLight->setTransform(utils::Transform(1.f, glm::quat(0.888179f, -0.260215f, 0.363439f, 0.106479f), glm::vec3(2.78139, 2.08128, 2.13792)));
//    scene0->sceneRootNode()->attach(spotLight);

    auto iblLight = std::make_shared<core::IBLLightNode>("");
    scene0->sceneRootNode()->attach(iblLight);

    auto planeDrawableNode = std::make_shared<core::DrawableNode>("");
    planeDrawableNode->addDrawable(planeDrawable);
    scene0->sceneRootNode()->attach(planeDrawableNode);

    auto teapotDrawableNode = std::make_shared<core::DrawableNode>("");
    teapotDrawableNode->addDrawable(teapotDrawable);
    scene0->sceneRootNode()->attach(teapotDrawableNode);

    auto boxDrawableNode = std::make_shared<core::DrawableNode>("");
    boxDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(0.2f, 0.5f, 0.42f)) *
                                  utils::Transform::fromRotation(glm::quat(glm::vec3(-0.4f, 0.3f, 0.0f))));
    boxDrawableNode->addDrawable(boxDrawable);
    //scene0->sceneRootNode()->attach(boxDrawableNode);

    auto tetraDrawableNode = std::make_shared<core::DrawableNode>("");
    tetraDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(-0.6f, 0.21f, -1.0f)));
    tetraDrawableNode->addDrawable(tetraDrawable);
    //scene0->sceneRootNode()->attach(tetraDrawableNode);

    auto sphereDrawableNode = std::make_shared<core::DrawableNode>("");
    sphereDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(1.4f, 0.4f, -1.f)));
    sphereDrawableNode->addDrawable(sphereDrawable);
    //scene0->sceneRootNode()->attach(sphereDrawableNode);

    auto suzanneDrawableNode = std::make_shared<core::DrawableNode>("");
    suzanneDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(0.3f, 0.25f, -1.8f)) *
                                      utils::Transform::fromRotation(glm::quat(glm::vec3(-0.7f, 0.5f, 0.0f))));
    suzanneDrawableNode->addDrawable(suzanneDrawable);
    //scene0->sceneRootNode()->attach(suzanneDrawableNode);

    auto bunnyDrawableNode = std::make_shared<core::DrawableNode>("");
    bunnyDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(1.3f, 0.01f, 1.0f)) *
                                      utils::Transform::fromRotation(glm::quat(glm::vec3(0.0f, -0.5f, 0.0f))));
    bunnyDrawableNode->addDrawable(bunnyDrawable);
    //scene0->sceneRootNode()->attach(bunnyDrawableNode);

    //scene0->sceneRootNode()->setTransform(utils::Transform::fromRotation(glm::quat(glm::vec3(0.0f, 1.57f, 0.0f))));
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return std::dynamic_pointer_cast<core::GraphicsEngine>(engine(GraphicsEngineName));
}

void TestApplication::doUpdate(uint64_t time, uint32_t dt)
{
}
