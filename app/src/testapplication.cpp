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
#include <core/frustumcullingvisitor.h>
#include <core/igraphicsrenderer.h>
#include <core/rendersurface.h>

#include "testapplication.h"

using namespace simplex;

const std::string GraphicsEngineName = "Simplex3DGraphicsEngine";


TestApplication::TestApplication(std::shared_ptr<simplex::core::graphics::IRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
{   
    auto graphicsEngineInstance = std::make_shared<simplex::core::GraphicsEngine>(GraphicsEngineName, graphicsRenderer);
    registerEngine(graphicsEngineInstance);


    utils::MeshPainter teapotPainter(utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::Normal, {3u, utils::VertexComponentType::Single}},
                                                                 {utils::VertexAttribute::TexCoord, {2u, utils::VertexComponentType::Single}}
                                                                }));
    teapotPainter.drawTeapot();
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


        drawables[i] = graphicsEngineInstance->createStandardDrawable(
                    teapotVao,
                    glm::vec4(1.f), 1.f, 1.f,
                    bc, m, r, n);
    }

    auto scene0 = graphicsEngineInstance->addNewScene("Scene0");

    auto cameraNode0 = std::make_shared<core::CameraNode>(graphicsEngineInstance->defaultRenderSurface(), "");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(glm::vec3(2.5f, 2.2f, 2.5f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/7.f, glm::quarter_pi<float>(), 0.f))));
    scene0->sceneRootNode()->attach(cameraNode0);

    for (size_t i = 0; i < drawables.size(); ++i)
    {
        auto drawableNode = std::make_shared<core::DrawableNode>("");
        drawableNode->addDrawable(drawables[i]);
        drawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(i/2 ? 1.f : -1.f, 0.0f, i%2 ? 1.f : -1.f)));
        scene0->sceneRootNode()->attach(drawableNode);
    }
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return std::dynamic_pointer_cast<core::GraphicsEngine>(engine(GraphicsEngineName));
}

void TestApplication::doUpdate(uint64_t time, uint32_t)
{
}
