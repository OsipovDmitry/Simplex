#include <algorithm>

#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/frustum.h>

#include <core/igraphicsrenderer.h>
#include <core/renderinfo.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/coloreddrawable.h>
#include <core/nodevisitor.h>
#include <core/collectorvisitor.h>

#include "graphicsengineprivate.h"
#include "cameranodeprivate.h"
#include "coloreddrawableprivate.h"
#include "nodeupdatevisitor.h"
#include "znearfarnodevisitor.h"
#include "drawablenoderenderer.h"

namespace simplex
{
namespace core
{

const char colorVertexShaderSource[] {
    "#version 450\n"
    "layout(location = 0) in vec3 a_position;\n"
    "uniform mat4 u_modelViewProjectionMatrix;\n"
    "void main() {\n"
    "   gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0f);\n"
    "}"
};
const char colorFragmentShaderSource[] {
    "#version 450\n"
    "uniform vec4 u_color;\n"
    "layout(location = 0) out vec4 color;\n"
    "void main() {\n"
    "  color = u_color;\n"
    "}"
};

float GraphicsEngine::s_minZNear = 0.5f;
float GraphicsEngine::s_maxZFat = 100000.0f;

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<IGraphicsRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name, renderer))
{
    auto boundingBoxMesh = std::make_shared<utils::Mesh>();
    boundingBoxMesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
    utils::MeshPainter(boundingBoxMesh).drawBoundingBox();

    NodePrivate::boundingBoxVertexArray() = renderer->createVertexArray(boundingBoxMesh);

    auto cameraMesh = std::make_shared<utils::Mesh>();
    cameraMesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
    utils::MeshPainter painter(cameraMesh);
    painter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(3.f)));
    painter.drawCamera();

    CameraNodePrivate::cameraVertexArray() = renderer->createVertexArray(cameraMesh);

    ColoredDrawablePrivate::coloredRenderProgram() = renderer->createRenderProgram(colorVertexShaderSource, colorFragmentShaderSource);

    LOG_INFO << "Engine \"" << GraphicsEngine::name() << "\" has been created";
}

GraphicsEngine::~GraphicsEngine()
{
    LOG_INFO << "Engine \"" << GraphicsEngine::name() << "\" has been destroyed";
}

const std::string &GraphicsEngine::name() const
{
    return m_->name();
}

void GraphicsEngine::update(uint64_t time, uint32_t dt)
{
    auto renderer = m_->renderer();
    float aspectRatio = static_cast<float>(renderer->width()) / static_cast<float>(renderer->height());

    auto sceneList = scenes();
    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObject::Comparator());

    for (auto &scene : sceneList)
    {
        auto rootNode = scene->sceneRootNode();

        NodeUpdateVisitor nodeUpdateVisitor(time ,dt);
        rootNode->accept(nodeUpdateVisitor);

        // update lights and shadows

        CollectorVisitor<CameraNode> cameraNodeCollector;
        rootNode->accept(cameraNodeCollector);
        std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObject::Comparator());

        std::vector<std::array<float, 2>> cameraZPlanes(cameraNodeCollector.nodes().size());
        for (size_t i = 0; i < cameraNodeCollector.nodes().size(); ++i)
        {
            const auto &camera = cameraNodeCollector.nodes()[i];

            ZNearFarNodeVisitor zNearFarNodeVisitor(utils::OpenFrustum(camera->projectionMatrix(aspectRatio, 0.f, 1.f) *
                                                                       camera->globalTransform().inverted()));
            rootNode->accept(zNearFarNodeVisitor);
            cameraZPlanes[i] = zNearFarNodeVisitor.zNearFar();
        }

        for (size_t i = 0; i < cameraNodeCollector.nodes().size(); ++i)
        {
            const auto &camera = cameraNodeCollector.nodes()[i];

            if (!camera->isRenderingEnabled())
                continue;

            auto zNear = glm::max(s_minZNear, cameraZPlanes[i][0]);
            auto zFar = glm::min(s_maxZFat, cameraZPlanes[i][1]);

            RenderInfo renderInfo;
            renderInfo.setViewport(glm::uvec2(renderer->width(), renderer->height()));
            renderInfo.setViewMatrix(camera->globalTransform().inverted());
            renderInfo.setProjectionMatrix(camera->projectionMatrix(aspectRatio, zNear, zFar));

            renderer->clearRenderData();

            if (true)
            {
                for (size_t j = 0; j < cameraNodeCollector.nodes().size(); ++j)
                {
                    if (i == j) continue;

                    const auto &anotherCamera = cameraNodeCollector.nodes()[j];

                    renderer->addRenderData(anotherCamera->globalTransform(), anotherCamera->m().cameraDrawable());
                    renderer->addRenderData(anotherCamera->globalTransform() *
                                            glm::inverse(anotherCamera->projectionMatrix(aspectRatio, cameraZPlanes[j][0], cameraZPlanes[j][1])),
                                            anotherCamera->m().frustumDrawable());
                }
            }

            DrawableNodeRenderer drawableNodeRenderer(renderer, utils::Frustum(renderInfo.projectionMatrix() * renderInfo.viewMatrix()));
            rootNode->accept(drawableNodeRenderer);

            renderer->render(renderInfo);
        }
    }

    renderer->clearRenderData();
}

std::shared_ptr<IGraphicsRenderer> GraphicsEngine::graphicsRenderer() const
{
    return m_->renderer();
}

const std::vector<std::shared_ptr<Scene>> &GraphicsEngine::scenes() const
{
    return m_->scenes();
}

void GraphicsEngine::addScene(std::shared_ptr<Scene> scene)
{
    if (auto it = std::find(m_->scenes().begin(), m_->scenes().end(), scene); it != m_->scenes().end())
    {
        LOG_ERROR << "Engine \"" << name() << "\" already has scene \"" + scene->name() + "\"";
        return;
    }

    m_->scenes().push_back(scene);
}

void GraphicsEngine::removeScene(std::shared_ptr<Scene> scene)
{
    if (auto it = std::find(m_->scenes().begin(), m_->scenes().end(), scene); it != m_->scenes().end())
        m_->scenes().erase(it);
}

}
}
