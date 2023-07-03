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
#include <core/textureddrawable.h>
#include <core/frustumcullingvisitor.h>
#include <core/igraphicsrenderer.h>
#include <core/viewport.h>

#include "testapplication.h"

using namespace simplex;

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>("Simplex3DGraphicsEngine", graphicsRenderer))
{   
    registerEngine(m_graphicsEngine);

    std::vector<std::shared_ptr<core::IGraphicsRenderer::Texture>> textures {
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex1.jpg")),
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex2.jpg")),
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex3.jpg"))
    };

    for (auto t : textures)
    {
        t->setWrapMode(core::IGraphicsRenderer::Texture::WrapMode::Repeat);
        t->setFiltering(core::IGraphicsRenderer::Texture::Filtering::Trilinear);
    }

    std::array<std::shared_ptr<core::DrawableBase>, 5u> drawables;
    utils::MeshPainter meshPainter;
    for (size_t i = 0; i < drawables.size(); ++i)
    {
        auto mesh = std::make_shared<utils::Mesh>();
        mesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
        mesh->attachVertexBuffer(utils::VertexAttribute::Normal, std::make_shared<utils::VertexBuffer>(0u, 3u));
        mesh->attachVertexBuffer(utils::VertexAttribute::TexCoord, std::make_shared<utils::VertexBuffer>(0u, 2u));
        mesh->attachVertexBuffer(utils::VertexAttribute::Color, std::make_shared<utils::VertexBuffer>(0u, 3u));
        meshPainter.setVertexTransform(glm::mat4(1.f));
        meshPainter.setTexCoordTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(1.f)));
        meshPainter.setMesh(mesh);
        if (i == 0u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.f, .3f, 1.f, 1.f));
            meshPainter.setVertexTransform(glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.25f, 0.f)));
            meshPainter.drawTeapot();
        }
        else if (i == 1u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.0f, .2f, .6f, 1.f));
            meshPainter.drawCube();
        }
        else if (i == 2u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.f, .6f, 0.f, 1.f));
            meshPainter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(.6f)));
            meshPainter.drawSphere(16);
        }
        else if (i == 3u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, .9f, .8f, 1.f));
            meshPainter.drawTetrahedron();
        }
        else if (i == 4u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, 1.f, 0.f, 1.f));
            meshPainter.drawMonkey();
        }

        auto texturedDrawable = std::make_shared<core::TexturedDrawable>(graphicsRenderer->createVertexArray(mesh));
        texturedDrawable->setTexture(textures[i%3]);

        drawables[i] = texturedDrawable;
    }

    const uint32_t textureSize = 1024;

    auto colorBuffer = graphicsRenderer->createTexture2DEmpty(textureSize, textureSize, core::IGraphicsRenderer::RenderSurface::InternalFormat::RGBA8);
    auto depthBuffer = graphicsRenderer->createRenderBuffer(textureSize, textureSize, core::IGraphicsRenderer::RenderSurface::InternalFormat::Depth16);
    auto fb = graphicsRenderer->createFrameBuffer();
    fb->attach(core::IGraphicsRenderer::FrameBuffer::Attachment::Color0, colorBuffer);
    fb->attach(core::IGraphicsRenderer::FrameBuffer::Attachment::Depth, depthBuffer);
    fb->setClearColor(0, glm::vec4(1.f, .5f, .5f, 1.f));

    m_scene0 = std::make_shared<core::Scene>("Scene0");
    m_graphicsEngine->addScene(m_scene0);

    auto cameraNode0 = std::make_shared<core::CameraNode>("CameraNode0");
    cameraNode0->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode0->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 0.f, 1.5f)));
    cameraNode0->setFrameBuffer(fb);
    cameraNode0->viewport()->setSizePolicy(core::Viewport::SizePolicy::Defined);
    cameraNode0->viewport()->setSize(glm::uvec4(0, 0, textureSize, textureSize));
    m_scene0->sceneRootNode()->attach(cameraNode0);

    auto drawableNode0 = std::make_shared<core::DrawableNode>("");
    drawableNode0->addDrawable(drawables[0]);
    m_scene0->sceneRootNode()->attach(drawableNode0);


    std::dynamic_pointer_cast<core::TexturedDrawable>(drawables[1])->setTexture(colorBuffer);

    m_scene1 = std::make_shared<core::Scene>("Scene0");
    m_graphicsEngine->addScene(m_scene1);

    auto cameraNode1 = std::make_shared<core::CameraNode>("CameraNode0");
    cameraNode1->setPerspectiveProjection(glm::pi<float>() / 3.f);
    cameraNode1->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 1.0f, 1.8f)) *
                              utils::Transform::fromRotation(glm::quat(glm::vec3(-glm::pi<float>()/7.f, 0.f, 0.f))));
    cameraNode1->frameBuffer()->setClearColor(0, glm::vec4(.5f, .5f, 1.f, 1.f));
    m_scene1->sceneRootNode()->attach(cameraNode1);

    auto drawableNode1 = std::make_shared<core::DrawableNode>("");
    drawableNode1->addDrawable(drawables[1]);
    m_scene1->sceneRootNode()->attach(drawableNode1);

}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return m_graphicsEngine;
}

void TestApplication::doUpdate(uint64_t time, uint32_t)
{
    m_scene0->sceneRootNode()->children()[1]->setTransform(utils::Transform::fromRotation(
                                                               glm::quat(time * 0.001f * glm::vec3(glm::half_pi<float>(), glm::pi<float>(), 0.f))));

    m_scene1->sceneRootNode()->children()[1]->setTransform(utils::Transform::fromRotation(glm::quat(time * 0.0001f *
                                                                                                    glm::vec3(0.f, glm::pi<float>(), 0.f))));
}
