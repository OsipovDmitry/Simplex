#include <algorithm>

#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/igraphicsrenderer.h>
#include <core/renderinfo.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/drawablenode.h>
#include <core/drawablebase.h>

#include "graphicsengineprivate.h"
#include "nodeupdatevisitor.h"
#include "drawablenodecollector.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<IGraphicsRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name, renderer))
{
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

    auto cameraMesh = std::make_shared<utils::Mesh>();
    cameraMesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
    cameraMesh->attachVertexBuffer(utils::VertexAttribute::Normal, std::make_shared<utils::VertexBuffer>(0u, 3u));
    cameraMesh->attachVertexBuffer(utils::VertexAttribute::Color, std::make_shared<utils::VertexBuffer>(0u, 3u));
    utils::MeshPainter painter(cameraMesh);
    painter.setDefaultValue(utils::VertexAttribute::Color, glm::vec4(1.f, 0.f, 0.f, 1.f));
    painter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(3.f)));
    painter.drawCamera();

    auto renderer = m_->renderer();

    auto nodeUpdateVisitor = std::make_shared<NodeUpdateVisitor>(time ,dt);
    auto cameraNodeCollector = std::make_shared<NodeCollector<CameraNode>>();
    auto drawableNodeCollector = std::make_shared<DrawableNodeCollector>();

    auto sceneList = scenes();
    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObject::Comparator());
    for (auto &scene : sceneList)
    {
        auto rootNode = scene->sceneRootNode();

        rootNode->accept(nodeUpdateVisitor);

        cameraNodeCollector->clear();
        rootNode->accept(cameraNodeCollector);
        auto cameraList = cameraNodeCollector->nodes();
        std::stable_sort(cameraList.begin(), cameraList.end(), utils::SortedObject::Comparator());

        // update lights and shadows

        for (auto &camera : cameraList)
        {
            if (!camera->isRenderingEnabled())
                continue;

            float aspectRatio = static_cast<float>(renderer->width()) / static_cast<float>(renderer->height());

            auto viewMatrix = glm::inverse(glm::mat4(camera->globalTransform()));
            auto projectionMatrix = camera->projectionMatrix(aspectRatio, 0.1f, 1000.0f);

            RenderInfo renderInfo;
            renderInfo.setViewport(glm::uvec2(renderer->width(), renderer->height()));
            renderInfo.setViewMatrix(viewMatrix);
            renderInfo.setProjectionMatrix(projectionMatrix);

            drawableNodeCollector->clear();
            rootNode->accept(drawableNodeCollector);

            renderer->clearRenderData();
            for (auto &c : cameraList)
                if (c != camera)
                    renderer->addRenderData(c->globalTransform(), std::make_shared<DrawableBase>(drawableNodeCollector->nodes().front()->drawables().begin()->get()->renderProgram(),
                                                                                                 m_->renderer()->createVertexArray(cameraMesh)));
            for (auto &drawableNode : drawableNodeCollector->nodes())
                for (const auto &drawable : drawableNode->drawables())
                    renderer->addRenderData(drawableNode->globalTransform(), drawable);
            renderer->render(renderInfo);
        }
    }

    renderer->clearRenderData();
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
