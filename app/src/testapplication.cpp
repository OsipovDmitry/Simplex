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
#include <core/drawablenode.h>
#include <core/igraphicsrenderer.h>
#include <core/standarddrawable.h>
#include <core/frustumcullingnodevisitor.h>
#include <core/igraphicsrenderer.h>

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

    utils::MeshPainter planePainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                  {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    planePainter.setVertexTransform(glm::rotate(glm::mat4x4(1.f), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) *
                                    glm::scale(glm::mat4x4(1.f), glm::vec3(10.f)));
    planePainter.setTexCoordTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(10.f)));
    planePainter.drawPlane();
    auto planeDrawable = std::make_shared<core::StandardDrawable>(
                graphicsRenderer->createVertexArray(planePainter.mesh()),
                glm::vec4(glm::vec3(1.f), 1.f), 1.f, 1.f,
                graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/res/textures/brick.jpg")),
                nullptr,
                nullptr,
                nullptr);

    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    teapotPainter.drawTeapot();
    auto teapotDrawable = std::make_shared<core::StandardDrawable>(
                graphicsRenderer->createVertexArray(teapotPainter.mesh()),
                glm::vec4(glm::vec3(1.f), 0.2f), 1.f, 1.f,
                graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/res/textures/glass0.jpg")),
                nullptr,
                nullptr,
                nullptr);

    utils::MeshPainter boxPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    boxPainter.setVertexTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(0.6f, 1.0f, 0.1f)));
    boxPainter.drawCube();
    auto boxDrawable = std::make_shared<core::StandardDrawable>(
                graphicsRenderer->createVertexArray(boxPainter.mesh()),
                glm::vec4(glm::vec3(1.f), 0.3f), 1.f, 1.f,
                graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/res/textures/glass1.jpg")),
                nullptr,
                nullptr,
                nullptr);

    utils::MeshPainter tetraPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    tetraPainter.setVertexTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(0.8f)));
    tetraPainter.drawTetrahedron();
    auto tetraDrawable = std::make_shared<core::StandardDrawable>(
                graphicsRenderer->createVertexArray(tetraPainter.mesh()),
                glm::vec4(glm::vec3(1.f), 0.3f), 1.f, 1.f,
                graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/res/textures/glass2.jpg")),
                nullptr,
                nullptr,
                nullptr);

    utils::MeshPainter monkeyPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoords, {2u, utils::VertexComponentType::Single}}
                                                                }));
    monkeyPainter.setVertexTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(0.4f)));
    monkeyPainter.drawSphere(32);
    auto monkeyDrawable = std::make_shared<core::StandardDrawable>(
                graphicsRenderer->createVertexArray(monkeyPainter.mesh()),
                glm::vec4(glm::vec3(1.f), 0.4f), 1.f, 1.f,
                graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/res/textures/glass3.jpg")),
                nullptr,
                nullptr,
                nullptr);

    auto scene0 = graphicsEngineInstance->addNewScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(5.0f * glm::vec3(2.5f, 0.5f, 2.5f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/9.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

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
    scene0->sceneRootNode()->attach(boxDrawableNode);

    auto tetraDrawableNode = std::make_shared<core::DrawableNode>("");
    tetraDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(-0.6f, 0.21f, -1.0f)));
    tetraDrawableNode->addDrawable(tetraDrawable);
    scene0->sceneRootNode()->attach(tetraDrawableNode);

    auto monkeyDrawableNode = std::make_shared<core::DrawableNode>("");
    monkeyDrawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(1.4f, 0.4f, -1.f)));
    monkeyDrawableNode->addDrawable(monkeyDrawable);
    scene0->sceneRootNode()->attach(monkeyDrawableNode);

}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return std::dynamic_pointer_cast<core::GraphicsEngine>(engine(GraphicsEngineName));
}

void TestApplication::doUpdate(uint64_t time, uint32_t dt)
{
}
