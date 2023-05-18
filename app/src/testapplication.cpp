#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/frustum.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/glm/gtc/matrix_transform.hpp>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/igraphicsrenderer.h>
#include <core/drawablebase.h>
#include <core/frustumcullingvisitor.h>

#include "testapplication.h"

using namespace simplex;

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>("Simplex3DGraphicsEngine", graphicsRenderer))
{   
    registerEngine(m_graphicsEngine);

    const std::string vertexShaderSource = {
        "#version 450\n"
        "layout(location = 0) in vec3 a_position;\n"
        "layout(location = 1) in vec3 a_normal;\n"
        "layout(location = 6) in vec3 a_color;\n"
        "uniform mat4 u_modelViewProjectionMatrix;\n"
        "out vec3 normal;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   normal = a_normal;\n"
        "   color = vec4(a_color, 1.0f);\n"
        "   gl_Position = u_modelViewProjectionMatrix * vec4(a_position.xyz, 1.0f);\n"
        "}"
    };
    const std::string fragmentShaderSource = {
        "#version 450\n"
        "in vec3 normal;"
        "in vec4 color;\n"
        "layout(location = 0) out vec4 ocolor;\n"
        "void main() {\n"
        "  vec3 n = normalize(normal);\n"
        "  vec3 l = normalize(vec3(0.3f, 1.0f, -0.7f));\n"
        "  float d = max(0.4f, dot(n, l));\n"
        "  ocolor = vec4(color.rgb * d, 1.f);\n"
        "}"
    };
    auto renderProgram = graphicsRenderer->createRenderProgram(vertexShaderSource, fragmentShaderSource);

    m_scene = std::make_shared<core::Scene>("Scene0");
    m_graphicsEngine->addScene(m_scene);

    auto sceneRootNode = m_scene->sceneRootNode();

    std::array<std::shared_ptr<core::DrawableBase>, 5u> drawables;
    utils::MeshPainter meshPainter;
    for (size_t i = 0; i < drawables.size(); ++i)
    {
        auto mesh = std::make_shared<utils::Mesh>();
        mesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
        mesh->attachVertexBuffer(utils::VertexAttribute::Normal, std::make_shared<utils::VertexBuffer>(0u, 3u));
        mesh->attachVertexBuffer(utils::VertexAttribute::Color, std::make_shared<utils::VertexBuffer>(0u, 3u));
        meshPainter.setVertexTransform(glm::mat4(1.f));
        meshPainter.setMesh(mesh);
        if (i == 0u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, .9f, .8f, 1.f));
            meshPainter.drawTetrahedron();
        }
        else if (i == 1u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.0f, .2f, .6f, 1.f));
            meshPainter.drawCube();
        }
        else if (i == 2u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, 1.f, 0.f, 1.f));
            meshPainter.drawMonkey();
        }
        else if (i == 3u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.f, .3f, 1.f, 1.f));
            meshPainter.drawTeapot();
        }
        else if (i == 4u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.f, .6f, 0.f, 1.f));
            meshPainter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(.6f)));
            meshPainter.drawSphere(16);
        }

        drawables[i] = std::make_shared<core::DrawableBase>(renderProgram, graphicsRenderer->createVertexArray(mesh));
    }

    for (int z = -4; z <= 4; ++z)
        for (int x = -4; x <= 4; ++x)
        {
            auto drawableNode = std::make_shared<core::DrawableNode>("");
            drawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(x*2.f, 0.f, z*2.f)));
            drawableNode->addDrawable(drawables[rand() % 5]);
            sceneRootNode->attach(drawableNode);

            m_drawableNodes.push_back(drawableNode);
        }


    m_cameraNode1 = std::make_shared<core::CameraNode>("CameraNode1");
    m_cameraNode1->setPerspectiveProjection(0.125f * glm::pi<float>());
    m_cameraNode1->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 2.f, 10.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.25f*glm::quarter_pi<float>(), 0.f, 0.f))));
    sceneRootNode->attach(m_cameraNode1);

    m_cameraNode2 = std::make_shared<core::CameraNode>("CameraNode2");
    m_cameraNode2->setPerspectiveProjection(glm::quarter_pi<float>());
    m_cameraNode2->setTransform(utils::Transform::fromTranslation(glm::vec3(13.f, 6.f, -13.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.3f*glm::quarter_pi<float>(), 3.f*glm::quarter_pi<float>(), 0.f))));
    sceneRootNode->attach(m_cameraNode2);
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return m_graphicsEngine;
}

void TestApplication::doUpdate(uint64_t, uint32_t)
{


}
