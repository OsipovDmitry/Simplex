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

#include "testapplication.h"

using namespace simplex;

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>("Simplex3DGraphicsEngine", graphicsRenderer))
{   
    registerEngine(m_graphicsEngine);

    m_scene = std::make_shared<core::Scene>("Scene0");
    m_graphicsEngine->addScene(m_scene);

    auto sceneRootNode = m_scene->sceneRootNode();

    std::vector<std::shared_ptr<core::IGraphicsRenderer::Texture>> textures {
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex1.jpg")),
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex2.jpg")),
        graphicsRenderer->createTexture2D(utils::Image::loadImage("D:/tex3.jpg"))
    };

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
            meshPainter.setTexCoordTransform(glm::scale(glm::mat4x4(1.f), glm::vec3(2.f)));
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

    for (int x = -1; x <= 1; ++x)
    {
        auto drawableNode = std::make_shared<core::DrawableNode>("");
        drawableNode->addDrawable(drawables[x+1]);
        sceneRootNode->attach(drawableNode);

        m_drawableNodes.push_back(drawableNode);
    }


    m_cameraNode1 = std::make_shared<core::CameraNode>("CameraNode1");
    m_cameraNode1->setPerspectiveProjection(0.125f * glm::pi<float>());
    m_cameraNode1->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 2.f, 8.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.25f*glm::quarter_pi<float>(), 0.f, 0.f))));
    sceneRootNode->attach(m_cameraNode1);

    m_cameraNode2 = std::make_shared<core::CameraNode>("CameraNode2");
    m_cameraNode2->setPerspectiveProjection(glm::quarter_pi<float>());
    m_cameraNode2->setTransform(utils::Transform::fromTranslation(glm::vec3(6.f, 4.f, -6.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.3f*glm::quarter_pi<float>(), 3.f*glm::quarter_pi<float>(), 0.f))));
    sceneRootNode->attach(m_cameraNode2);
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return m_graphicsEngine;
}

void TestApplication::doUpdate(uint64_t time, uint32_t)
{
    m_drawableNodes[0]->setTransform(utils::Transform::fromTranslation(glm::vec3(-2.f, 0.f, 0.f))*
                                     utils::Transform::fromRotation(glm::quat(glm::vec3(0.f, time * 0.001f, 0.f))));

    m_drawableNodes[1]->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 0.f, 0.f))*
                                     utils::Transform::fromRotation(glm::quat(-glm::vec3(0.f, time * 0.001f, 0.f))));

    m_drawableNodes[2]->setTransform(utils::Transform::fromTranslation(glm::vec3(+2.f, 0.f, 0.f))*
                                     utils::Transform::fromRotation(glm::quat(glm::vec3(0.f, time * 0.001f, 0.f))));

}
