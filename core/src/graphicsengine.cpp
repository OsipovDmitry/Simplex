#include <algorithm>

#include <utils/logger.h>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>

#include "graphicsengineprivate.h"
#include "nodeupdatevisitor.h"
#include "nodecollector.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<IGraphicsRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name, renderer))
{
    LOG_INFO << "Engine \"" << GraphicsEngine::name() << "\" has been created";
    addScene(std::make_shared<Scene>("StandardScene"));
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
    auto nodeUpdateVisitor = std::make_shared<NodeUpdateVisitor>(time ,dt);
    auto cameraNodeCollector = std::make_shared<NodeCollector<CameraNode>>();

    auto sceneList = scenes();
    std::stable_sort(sceneList.begin(), sceneList.end(), SortedObject::Comparator());
    for (auto &scene : sceneList)
    {
        auto rootNode = scene->sceneRootNode();

        rootNode->accept(nodeUpdateVisitor);

        cameraNodeCollector->clear();
        rootNode->accept(cameraNodeCollector);
        auto cameraList = cameraNodeCollector->nodes();
        std::stable_sort(cameraList.begin(), cameraList.end(), SortedObject::Comparator());


    }
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
