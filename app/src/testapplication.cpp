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


    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoord, {2u, utils::VertexComponentType::Single}}
                                                                }));
    teapotPainter.drawCube();
    auto teapotVao = graphicsRenderer->createVertexArray(teapotPainter.mesh());


    std::vector<std::shared_ptr<core::IDrawable>> drawables(4);
    for (size_t i = 0; i < drawables.size(); ++i)
    {
        std::shared_ptr<core::graphics::ITexture> bc, m, r, n;

        if (auto img = utils::Image::loadImage("D:/res/textures/tex" + std::to_string(i) + "/bc.jpg"); img)
        {
            bc = graphicsRenderer->createTexture2D(img);
            bc->setWrapMode(core::graphics::TextureWrapMode::Repeat);
            bc->setFiltering(core::graphics::TextureFiltering::Trilinear);
        }

        if (auto img = utils::Image::loadImage("D:/res/textures/tex" + std::to_string(i) + "/m.jpg"); img)
        {
            m = graphicsRenderer->createTexture2D(img);
            m->setWrapMode(core::graphics::TextureWrapMode::Repeat);
            m->setFiltering(core::graphics::TextureFiltering::Trilinear);
        }

        if (auto img = utils::Image::loadImage("D:/res/textures/tex" + std::to_string(i) + "/r.jpg"); img)
        {
            r = graphicsRenderer->createTexture2D(img);
            r->setWrapMode(core::graphics::TextureWrapMode::Repeat);
            r->setFiltering(core::graphics::TextureFiltering::Trilinear);
        }

        if (auto img = utils::Image::loadImage("D:/res/textures/tex" + std::to_string(i) + "/n.jpg"); img)
        {
            n = graphicsRenderer->createTexture2D(img);
            n->setWrapMode(core::graphics::TextureWrapMode::Repeat);
            n->setFiltering(core::graphics::TextureFiltering::Trilinear);
        }


        drawables[i] = std::make_shared<core::StandardDrawable>(
                    teapotVao,
                    glm::vec4(1.f), 1.f, 1.f,
                    bc, m, r, n);
    }

    auto scene0 = graphicsEngineInstance->addNewScene(SceneName);

    auto cameraNode0 = std::make_shared<core::CameraNode>("");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(5.0f * glm::vec3(2.5f, 0.5f, 2.5f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/9.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

    for (size_t i = 0; i < drawables.size(); ++i)
    {
        auto drawableNode = std::make_shared<core::DrawableNode>("");
        drawableNode->addDrawable(drawables[i]);
        drawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(i/2 ? 1.f : -1.f, 0.0f, i%2 ? 1.f : -1.f)));
        scene0->sceneRootNode()->attach(drawableNode);
    }

    for (int32_t x = -9; x <= 10; ++x)
        for (int32_t z = -9; z <= 10; ++z)
        {
            if (glm::distance(glm::vec2(0.5f), glm::vec2(x, z)) < 1.f)
                continue;

            auto drawable = std::make_shared<core::StandardDrawable>(
                        teapotVao,
                        glm::vec4(random(.4f, .9f), random(.4f, .9f), random(.4f, .9f), 0.6f), 1.f, 1.f,
                        nullptr, nullptr, nullptr, nullptr);

            auto drawableNode = std::make_shared<core::DrawableNode>("");
            drawableNode->addDrawable(drawable);
            drawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(x * 2.0f - 1.0f, 0.0f, z * 2.0f - 1.0f)));
            scene0->sceneRootNode()->attach(drawableNode);
        }
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return std::dynamic_pointer_cast<core::GraphicsEngine>(engine(GraphicsEngineName));
}

void TestApplication::doUpdate(uint64_t time, uint32_t dt)
{
}
