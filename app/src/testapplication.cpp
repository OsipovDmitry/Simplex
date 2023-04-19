#include <utils/logger.h>
#include <utils/boundingbox.h>
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

#include "testapplication.h"

using namespace simplex;

TestApplication::TestApplication(std::shared_ptr<simplex::core::IGraphicsRenderer> graphicsRenderer)
    : simplex::core::ApplicationBase("TestApplication")
    , m_graphicsEngine(std::make_shared<simplex::core::GraphicsEngine>("Simplex3DGraphicsEngine", graphicsRenderer))
{   
    registerEngine(m_graphicsEngine);

    const std::string vertexShaderSource = {
        "#version 450\n"
        "layout(location = 0) in vec3 iposition;\n"
        "layout(location = 1) in vec3 inormal;\n"
        "layout(location = 6) in vec3 icolor;\n"
        "uniform mat4 u_modelMatrix;\n"
        "uniform mat4 u_viewMatrix;\n"
        "uniform mat4 u_projectionMatrix;\n"
        "out vec3 normal;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   normal = inormal;\n"
        "   color = vec4(icolor, 1.0f);\n"
        "   gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(iposition, 1.0f);\n"
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

    auto scene = std::make_shared<core::Scene>("Scene0");
    m_graphicsEngine->addScene(scene);

    auto sceneRootNode = scene->sceneRootNode();

    std::array<std::shared_ptr<utils::Mesh>, 5u> meshes;
    utils::MeshPainter meshPainter;
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        meshPainter.setVertexTransform(glm::mat4(1.f));
        meshes[i] = std::make_shared<utils::Mesh>();
        meshes[i]->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
        meshes[i]->attachVertexBuffer(utils::VertexAttribute::Normal, std::make_shared<utils::VertexBuffer>(0u, 3u));
        meshes[i]->attachVertexBuffer(utils::VertexAttribute::Color, std::make_shared<utils::VertexBuffer>(0u, 3u));
        meshPainter.setMesh(meshes[i]);
        if (i == 0u)
        {
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, 1.f, .0f, 1.f));
            meshPainter.drawTriangle();
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
            meshPainter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(0.f, 1.f, 0.f, 1.f));
            meshPainter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(.6f)));
            meshPainter.drawSphere();
        }
    }

    for (int z = -4; z <= 4; ++z)
        for (int x = -4; x <= 4; ++x)
        {
            auto drawableNode = std::make_shared<core::DrawableNode>("");
            drawableNode->setTransform(utils::Transform::fromTranslation(glm::vec3(x*2.f, 0.f, z*2.f)));
            sceneRootNode->attach(drawableNode);

            auto vao = graphicsRenderer->createVertexArray(meshes[rand() % meshes.size()]);
            drawableNode->addDrawable(std::make_shared<core::DrawableBase>(renderProgram, vao));
        }


    auto cameraNode1 = std::make_shared<core::CameraNode>("CameraNode1");
    cameraNode1->setTransform(utils::Transform::fromTranslation(glm::vec3(0.f, 2.f, 10.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.5f*glm::quarter_pi<float>(), 0.f, 0.f))));
    sceneRootNode->attach(cameraNode1);

    auto cameraNode2 = std::make_shared<core::CameraNode>("CameraNode2");
    cameraNode2->setPerspectiveProjection(glm::quarter_pi<float>());
    cameraNode2->setTransform(utils::Transform::fromTranslation(glm::vec3(13.f, 6.f, -13.f)) *
                             utils::Transform::fromRotation(glm::quat(glm::vec3(-.3f*glm::quarter_pi<float>(), 3.f*glm::quarter_pi<float>(), 0.f))));
    sceneRootNode->attach(cameraNode2);
}

std::shared_ptr<core::GraphicsEngine> TestApplication::graphicsEngine()
{
    return m_graphicsEngine;
}
