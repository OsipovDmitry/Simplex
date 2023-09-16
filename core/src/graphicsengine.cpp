#include <algorithm>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/image.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/frustum.h>
#include <utils/shader.h>

#include <core/igraphicsrenderer.h>
#include <core/renderinfo.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/lightnode.h>
#include <core/drawablenode.h>
#include <core/standarddrawable.h>
#include <core/nodevisitor.h>
#include <core/collectorvisitor.h>
#include <core/texturesmanager.h>
#include <core/programsmanager.h>

#include "graphicsengineprivate.h"
#include "scenerootnodeprivate.h"
#include "nodeupdatevisitor.h"
#include "znearfarnodevisitor.h"
#include "drawablenodevisitor.h"


#include <core/uniform.h>

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    m_->renderer() = renderer;

    auto texturesManager = std::make_shared<TexturesManager>(renderer);
    m_->texturesManager() = texturesManager;

    auto programsManager = std::make_shared<ProgramsManager>(renderer);
    m_->programsManager() = programsManager;

    auto screenQuadMeshPainter = utils::MeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}}));
    screenQuadMeshPainter.drawScreenQuad();
    m_->screenQuadDrawable() = std::make_shared<DrawableBase>(renderer->createVertexArray(screenQuadMeshPainter.mesh()));

    m_->OITClearComputeProgram() = programsManager->loadOrGetOITClearPassComputeProgram();
    m_->OITSortNodesComputeProgram() = programsManager->loadOrGetOITSortNodesPassComputeProgram();

    m_->finalRenderProgram() = programsManager->loadOrGetFinalPassRenderProgram(
                m_->screenQuadDrawable()->vertexArray()->vertexAttributesSet());

    m_->defaultBaseColor() = glm::vec4(1.f, 1.f, 1.f, 1.f);
    m_->defaultMetallness() = 1.f;
    m_->defaultRoughness() = 1.f;

    static const uint32_t s_OITMaxNumNodes = 1920 * 1080 * 4;
    static const size_t s_OITDataBufferSize = sizeof(s_OITMaxNumNodes) + s_OITMaxNumNodes * 16u; // 16 bytes per node
    auto OITNodesBuffer = renderer->createBuffer(s_OITDataBufferSize);
    reinterpret_cast<uint32_t*>(OITNodesBuffer->map(graphics::IBuffer::MapAccess::WriteOnly, 0u, sizeof(uint32_t))->get())[0] = s_OITMaxNumNodes;
    m_->OITNodesBuffer() = renderer->createBufferRange(OITNodesBuffer, 0u);
    m_->OITNodesCounter() = renderer->createBufferRange(renderer->createBuffer(sizeof(uint32_t)), 0u);

    utils::MeshPainter pointLightAreaMeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}}));
    m_->lightAreaDrawables()[castFromLightNodeType(LightNodeType::Point)] = std::make_shared<DrawableBase>(
                renderer->createVertexArray(pointLightAreaMeshPainter.drawSphere(8u).mesh()));

    utils::MeshPainter spotLightAreaMeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}}));
    m_->lightAreaDrawables()[castFromLightNodeType(LightNodeType::Spot)] = std::make_shared<DrawableBase>(
                renderer->createVertexArray(spotLightAreaMeshPainter.drawCone(8u).mesh()));

    utils::MeshPainter directionalLightAreaMeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}}));
    m_->lightAreaDrawables()[castFromLightNodeType(LightNodeType::Directional)] = std::make_shared<DrawableBase>(
                renderer->createVertexArray(directionalLightAreaMeshPainter.drawCube().mesh()));

//    auto boundingBoxMesh = std::make_shared<utils::Mesh>();
//    boundingBoxMesh->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
//    utils::MeshPainter(boundingBoxMesh).drawBoundingBox();
//    auto boundingBoxVertexArray = renderer->createVertexArray(boundingBoxMesh);

//    m_->nodeBoundingBoxDrawable() = std::make_shared<StandardDrawable>(boundingBoxVertexArray);
//    m_->nodeBoundingBoxDrawable()->setColor(glm::vec4(.8f, .8f, .8f, 1.f));

//    m_->drawableNodeLocalBoundingBoxDrawable() = std::make_shared<StandardDrawable>(boundingBoxVertexArray);
//    m_->drawableNodeLocalBoundingBoxDrawable()->setColor(glm::vec4(.8f, .2f, .2f, 1.f));

//    m_->cameraNodeFrustumDrawable() = std::make_shared<StandardDrawable>(boundingBoxVertexArray);
//    m_->cameraNodeFrustumDrawable()->setColor(glm::vec4(.2f, .8f, .2f, 1.f));

//    utils::MeshPainter cameraPainter(std::make_shared<utils::Mesh>());
//    cameraPainter.mesh()->attachVertexBuffer(utils::VertexAttribute::Position, std::make_shared<utils::VertexBuffer>(0u, 3u));
//    cameraPainter.setVertexTransform(glm::scale(glm::mat4(1.f), glm::vec3(3.f)));
//    cameraPainter.drawCamera();

//    m_->cameraNodeCameraDrawable() = std::make_shared<StandardDrawable>(renderer->createVertexArray(cameraPainter.mesh()));
//    m_->cameraNodeCameraDrawable()->setColor(glm::vec4(.2f, .2f, .8f, 1.f));

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

    const auto &screenSize = renderer->viewportSize();

    auto sceneList = scenes();
    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObject::Comparator());

    for (auto &scene : sceneList)
    {
        auto rootNode = scene->sceneRootNode();

        // update nodes
        NodeUpdateVisitor nodeUpdateVisitor(time, dt);
        rootNode->accept(nodeUpdateVisitor);

        // update lights and shadows
        CollectorVisitor<LightNode> lightNodeCollector;
        rootNode->accept(lightNodeCollector);

        for (const auto &light : lightNodeCollector.nodes())
        {
            if (!light->isLightingEnabled())
                continue;

            // update shadow map
        }

        // render
        CollectorVisitor<CameraNode> cameraNodeCollector;
        rootNode->accept(cameraNodeCollector);
        std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObject::Comparator());

        for (const auto &camera : cameraNodeCollector.nodes())
        {
            if (!camera->isRenderingEnabled())
                continue;

            camera->resize(screenSize);

            const auto cameraViewportSize = camera->viewportSize();
            const auto cameraViewport = glm::uvec4(0u, 0u, cameraViewportSize);
            float aspectRatio = static_cast<float>(cameraViewportSize[0]) / static_cast<float>(cameraViewportSize[1]);
            const auto &cameraCullPlanesLimits = camera->cullPlanesLimits();

            ZNearFarNodeVisitor zNearFarNodeVisitor(utils::OpenFrustum(camera->projectionMatrix(aspectRatio, 0.f, 1.f) *
                                                                       camera->globalTransform().inverted()));
            rootNode->accept(zNearFarNodeVisitor);
            auto zNear = glm::max(cameraCullPlanesLimits[0], zNearFarNodeVisitor.zNearFar()[0] * 0.95f);
            auto zFar = glm::min(cameraCullPlanesLimits[1], zNearFarNodeVisitor.zNearFar()[1] * 1.05f);

            auto cameraProjectionMatrix = camera->projectionMatrix(aspectRatio, zNear, zFar);
            auto cameraViewMatrix = camera->globalTransform().inverted();

            DrawableNodeVisitor drawableNodeVisitor(utils::Frustum(cameraProjectionMatrix * cameraViewMatrix));
            rootNode->accept(drawableNodeVisitor);

            RenderInfo renderInfo;
            renderInfo.setViewMatrix(cameraViewMatrix);
            renderInfo.setProjectionMatrix(cameraProjectionMatrix);

            renderInfo.setGBufferMaps(camera->GFrameBufferColorMap0(),
                                      camera->GFrameBufferColorMap1(),
                                      camera->GFrameBufferDepthStencilMap());

            renderInfo.setOITNodesBuffer(m_->OITNodesBuffer());
            renderInfo.setOITNodesCounter(m_->OITNodesCounter());
            renderInfo.setOITIndicesImage(camera->OITIndicesImage());

            // render opaque geometry
            renderInfo.setFaceCulling(true);
            renderInfo.setDepthTest(true);
            renderInfo.setDepthMask(true);
            renderInfo.setColorMasks(true);

            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodeVisitor.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                    if (!drawable->isTransparent())
                        renderer->addRenderData(m_->programsManager()->loadOrGetOpaqueGeometryPassRenderProgram(
                                                    drawable->vertexArray()->vertexAttributesSet(),
                                                    drawable->PBRComponentsSet()),
                                                drawable,
                                                drawableNode->globalTransform());
            renderer->render(camera->GFrameBuffer(), renderInfo, cameraViewport);

            // reset OIT counter buffer
            auto nodecCounterBuffer = m_->OITNodesCounter()->buffer();
            if (auto nodesCounterBufferData = nodecCounterBuffer->map(graphics::IBuffer::MapAccess::WriteOnly); nodesCounterBufferData)
            {
                std::memset(nodesCounterBufferData->get(), 0, nodecCounterBuffer->size());
            }

            // clear OIT indices image
            renderer->compute(m_->OITClearComputeProgram(), renderInfo, glm::uvec3(cameraViewportSize, 1u));

            // render transparent geometry
            renderInfo.setFaceCulling(false);
            renderInfo.setDepthTest(true);
            renderInfo.setDepthMask(false);
            renderInfo.setColorMasks(false);

            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodeVisitor.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                    if (drawable->isTransparent())
                        renderer->addRenderData(m_->programsManager()->loadOrGetTransparentGeometryPassRenderProgram(
                                                    drawable->vertexArray()->vertexAttributesSet(),
                                                    drawable->PBRComponentsSet()),
                                                drawable,
                                                drawableNode->globalTransform());
            renderer->render(camera->OITFrameBuffer(), renderInfo, cameraViewport);

            // sort OIT nodes
            renderer->compute(m_->OITSortNodesComputeProgram(), renderInfo, glm::uvec3(cameraViewportSize, 1u));

            // render lights areas

            // render final
            renderInfo.setFaceCulling(false);
            renderInfo.setDepthTest(false);
            renderInfo.setColorMasks(true);

            renderer->clearRenderData();
            renderer->addRenderData(m_->finalRenderProgram(),
                                    m_->screenQuadDrawable());
            renderer->render(camera->finalFrameBuffer(), renderInfo, cameraViewport);

            renderer->blitFrameBuffer(camera->finalFrameBuffer(), renderer->defaultFrameBuffer(),
                                      cameraViewport,
                                      glm::uvec4(0u, 0u, screenSize),
                                      true, false, false);
        }
    }

    renderer->clearRenderData();
}

std::shared_ptr<graphics::IRenderer> GraphicsEngine::graphicsRenderer() const
{
    return m_->renderer();
}

std::shared_ptr<TexturesManager> GraphicsEngine::texturesManager() const
{
    return m_->texturesManager();
}

std::shared_ptr<ProgramsManager> GraphicsEngine::programsManager() const
{
    return m_->programsManager();
}

const std::vector<std::shared_ptr<Scene>> &GraphicsEngine::scenes() const
{
    return m_->scenes();
}

std::shared_ptr<Scene> GraphicsEngine::addNewScene(const std::string &name)
{
    auto scene = std::shared_ptr<Scene>(new Scene(shared_from_this(), name));
    scene->sceneRootNode()->m().scene() = scene;
    m_->scenes().push_back(scene);

    return scene;
}

void GraphicsEngine::removeScene(std::shared_ptr<Scene> scene)
{
    if (auto it = std::find(m_->scenes().begin(), m_->scenes().end(), scene); it != m_->scenes().end())
        m_->scenes().erase(it);
    else
        LOG_ERROR << "Graphics engine \"" << name() << "\" doesn't have scene \"" << scene->name() << "\"";
}

void GraphicsEngine::setF(int i)
{
    m_->screenQuadDrawable()->getOrCreateUserUniform("i") = makeUniform(i);
}

}
}
