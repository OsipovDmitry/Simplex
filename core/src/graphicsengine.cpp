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
#include <core/drawablenode.h>
#include <core/standarddrawable.h>
#include <core/nodevisitor.h>
#include <core/collectorvisitor.h>
#include <core/renderprogramsmanager.h>

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

const float s_minZNear = 0.5f;
const float s_maxZFar = 100000.0f;

const std::array<std::string, utils::numElementsVertexAttribute()> s_vertexAttributesLocations {
    std::string("POSITION_ATTRIBUTE"),
    std::string("NORMAL_ATTRIBUTE"),
    std::string("TEX_COORDS_ATTRIBUTE"),
    std::string("BONES_IDS_ATTRIBUTE"),
    std::string("BONES_WEIGHTS_ATTRIBUTE"),
    std::string("TANGENT_ATTRIBUTE"),
    std::string("COLOR_ATTRIBUTE")
};

const std::filesystem::path s_deferredGeometryPassVertexShader = "D:/res/shaders/deferred_geometry_pass.vert";
const std::filesystem::path s_deferredOpaqueGeometryPassFragmentShader = "D:/res/shaders/deferred_opaque_geometry_pass.frag";
const std::filesystem::path s_deferredTransparentGeometryPassFragmentShader = "D:/res/shaders/deferred_transparent_geometry_pass.frag";

const std::filesystem::path s_OITSortNodesVertexShader = "D:/res/shaders/oit_sort.vert";
const std::filesystem::path s_OITSortNodesFragmentShader = "D:/res/shaders/oit_sort.frag";

const std::filesystem::path s_finalPassVertexShader = "D:/res/shaders/final.vert";
const std::filesystem::path s_finalPassFragmentShader = "D:/res/shaders/final.frag";

class ScreeQuadFinalDrawable : public DrawableBase
{
public:
    explicit ScreeQuadFinalDrawable(std::shared_ptr<graphics::IVertexArray> vao)
        : DrawableBase(vao)
    {}

    void setRenderType(int i)
    {
        getOrCreateUserUniform("i")= makeUniform(i);
    }
};

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    m_->renderer() = renderer;

    auto renderProgramsManager = std::make_shared<RenderProgramsManager>(renderer);
    m_->renderProgramsManager() = renderProgramsManager;

    utils::ShaderDefines attribsDefines;
    for (size_t i = 0; i < s_vertexAttributesLocations.size(); ++i)
        attribsDefines[s_vertexAttributesLocations[i]] = std::to_string(i);

    m_->deferredOpaqueGeometryPassRenderProgram() = renderProgramsManager->loadOrGet(
                s_deferredGeometryPassVertexShader,
                s_deferredOpaqueGeometryPassFragmentShader,
                attribsDefines);

    m_->deferredTransparentGeometryPassRenderProgram() = renderProgramsManager->loadOrGet(
                s_deferredGeometryPassVertexShader,
                s_deferredTransparentGeometryPassFragmentShader,
                attribsDefines);

    m_->OITSortNodesRenderProgram() = renderProgramsManager->loadOrGet(
                s_OITSortNodesVertexShader,
                s_OITSortNodesFragmentShader,
                attribsDefines);

    m_->defaultVertexPosition() = glm::vec4(0.f, 0.f, 0.f, 1.f);
    m_->defaultVertexNormal() = glm::vec4(0.f, 0.f, 0.f, 0.f);
    m_->defaultVertexTexCoord() = glm::vec4(0.f, 0.f, 0.f, 0.f);
    m_->defaultVertexBonesIDs() = glm::uvec4(0u, 0u, 0u, 0u);
    m_->defaultVertexBonesWeights() = glm::vec4(1.f, 0.f, 0.f, 0.f);
    m_->defaultVertexTangent() = glm::vec4(1.f, 0.f, 0.f, 0.f);
    m_->defaultVertexColor() = glm::vec4(1.f, 1.f, 1.f, 1.f);

    m_->defaultBaseColor() = glm::vec4(1.f, 1.f, 1.f, 1.f);
    m_->defaultMetallness() = 1.f;
    m_->defaultRoughness() = 1.f;

    static const std::array<uint8_t, 4u> s_baseColorMapData {255u, 255u, 255u, 255u};
    m_->defaultBaseColorMap() = renderer->createTexture2D(
                utils::Image::createImage(1u, 1u, 4u, utils::PixelComponentType::Uint8, s_baseColorMapData.data()));

    static const std::array<uint8_t, 1u> s_metalnessMapData {255u};
    m_->defaultMetalnessMap() = renderer->createTexture2D(
                utils::Image::createImage(1u, 1u, 1u, utils::PixelComponentType::Uint8, s_metalnessMapData.data()));

    static const std::array<uint8_t, 1u> s_roughnessMapData {255u};
    m_->defaultRoughnessMap() = renderer->createTexture2D(
                utils::Image::createImage(1u, 1u, 1u, utils::PixelComponentType::Uint8, s_roughnessMapData.data()));

    static const std::array<uint8_t, 3u> s_normalMapData {128u, 128u, 255u};
    m_->defaultNormalMap() = renderer->createTexture2D(
                utils::Image::createImage(1u, 1u, 3u, utils::PixelComponentType::Uint8, s_normalMapData.data()));

    static glm::mat3x4 s_bonesBuffer(1.f);
    m_->defaultBonesBuffer() = renderer->createBufferRange(renderer->createBuffer(sizeof(s_bonesBuffer), glm::value_ptr(s_bonesBuffer)), 0u);

    static const uint32_t s_OITMaxNumNodes = 1920 * 1080 * 4;
    static const size_t s_OITDataBufferSize = sizeof(s_OITMaxNumNodes) + s_OITMaxNumNodes * 44; // 44 bytes per node
    auto OITNodesBuffer = renderer->createBuffer(s_OITDataBufferSize);
    reinterpret_cast<uint32_t*>(OITNodesBuffer->map(graphics::IBuffer::MapAccess::WriteOnly, 0u, sizeof(uint32_t))->get())[0] = s_OITMaxNumNodes;
    m_->OITNodesBuffer() = renderer->createBufferRange(OITNodesBuffer, 0u);
    m_->OITNodesCounter() = renderer->createBufferRange(renderer->createBuffer(sizeof(uint32_t)), 0u);

    auto screenQuadMeshPainter = utils::MeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}}));
    screenQuadMeshPainter.drawScreenQuad();
    auto screenQuadVertexArray = renderer->createVertexArray(screenQuadMeshPainter.mesh());

    m_->finalRenderProgram() = renderProgramsManager->loadOrGet(
                s_finalPassVertexShader,
                s_finalPassFragmentShader,
                attribsDefines);

    m_->screenQuadFinalDrawable() = std::make_shared<ScreeQuadFinalDrawable>(screenQuadVertexArray);

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

        NodeUpdateVisitor nodeUpdateVisitor(time ,dt);
        rootNode->accept(nodeUpdateVisitor);

        // update lights and shadows

        CollectorVisitor<CameraNode> cameraNodeCollector;
        rootNode->accept(cameraNodeCollector);
        std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObject::Comparator());

        for (size_t i = 0; i < cameraNodeCollector.nodes().size(); ++i)
        {
            const auto &camera = cameraNodeCollector.nodes()[i];

            if (!camera->isRenderingEnabled())
                continue;

            camera->resize(screenSize);

            const auto &cameraViewportSize = camera->viewportSize();
            float aspectRatio = static_cast<float>(cameraViewportSize.x) / static_cast<float>(cameraViewportSize.y);

            ZNearFarNodeVisitor zNearFarNodeVisitor(utils::OpenFrustum(camera->projectionMatrix(aspectRatio, 0.f, 1.f) *
                                                                       camera->globalTransform().inverted()));
            rootNode->accept(zNearFarNodeVisitor);
            auto zNear = glm::max(s_minZNear, zNearFarNodeVisitor.zNearFar()[0]);
            auto zFar = glm::min(s_maxZFar, zNearFarNodeVisitor.zNearFar()[1]);

            auto cameraProjectionMatrix = camera->projectionMatrix(aspectRatio, zNear, zFar);
            auto cameraViewMatrix = camera->globalTransform().inverted();

            DrawableNodeVisitor drawableNodeVisitor(utils::Frustum(cameraProjectionMatrix * cameraViewMatrix));
            rootNode->accept(drawableNodeVisitor);

            // render opaque geometry
            RenderInfo renderInfo;
            renderInfo.setDefaultVertexPosition(m_->defaultVertexPosition());
            renderInfo.setDefaultVertexNormal(m_->defaultVertexNormal());
            renderInfo.setDefaultVertexTexCoord(m_->defaultVertexTexCoord());
            renderInfo.setDefaultVertexBonesIDs(m_->defaultVertexBonesIDs());
            renderInfo.setDefaultVertexBonesWeights(m_->defaultVertexBonesWeights());
            renderInfo.setDefaultVertexTangent(m_->defaultVertexTangent());
            renderInfo.setDefaultVertexColor(m_->defaultVertexColor());
            renderInfo.setDefaultBaseColor(m_->defaultBaseColor());
            renderInfo.setDefaultMetalness(m_->defaultMetallness());
            renderInfo.setDefaultRoughness(m_->defaultRoughness());
            renderInfo.setDefaultBaseColorMap(m_->defaultBaseColorMap());
            renderInfo.setDefaultMetalnessMap(m_->defaultMetalnessMap());
            renderInfo.setDefaultRoughnessMap(m_->defaultRoughnessMap());
            renderInfo.setDefaultNormalMap(m_->defaultNormalMap());
            renderInfo.setDefaultBonesBuffer(m_->defaultBonesBuffer());

            renderInfo.setViewMatrix(cameraViewMatrix);
            renderInfo.setProjectionMatrix(cameraProjectionMatrix);
            renderInfo.setViewport(glm::uvec4(0u, 0u, cameraViewportSize));
            renderInfo.setRenderProgram(m_->deferredOpaqueGeometryPassRenderProgram());
            renderInfo.setFrameBuffer(camera->GFrameBuffer());

            renderInfo.setDepthTest(true);
            renderInfo.setDepthMask(true);
            renderInfo.setColorMasks(true);

            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodeVisitor.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                    if (!drawable->isTransparent())
                        renderer->addRenderData(drawableNode->globalTransform(), drawable);
            renderer->render(renderInfo);

            {
                // reset OIT counter buffer
                auto nodecCounterBuffer = m_->OITNodesCounter()->buffer();
                auto nodesCounterBufferData = nodecCounterBuffer->map(graphics::IBuffer::MapAccess::WriteOnly);
                std::memset(nodesCounterBufferData->get(), 0, nodecCounterBuffer->size());

                // reset OIT indices image
                renderInfo.setDepthTest(false);
                renderInfo.setColorMasks(true);
                renderInfo.setFrameBuffer(camera->OITClearIndicesFrameBuffer());
                renderer->clearRenderData();
                renderer->render(renderInfo);

            }

            // render transparent geometry
            renderInfo.setRenderProgram(m_->deferredTransparentGeometryPassRenderProgram());
            renderInfo.setFrameBuffer(camera->OITFrameBuffer());

            renderInfo.setOITNodesBuffer(m_->OITNodesBuffer());
            renderInfo.setOITNodesCounter(m_->OITNodesCounter());
            renderInfo.setOITIndicesImage(camera->OITIndicesImage());

            renderInfo.setDepthTest(true);
            renderInfo.setDepthMask(false);
            renderInfo.setColorMasks(false);

            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodeVisitor.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                    if (drawable->isTransparent())
                        renderer->addRenderData(drawableNode->globalTransform(), drawable);
            renderer->render(renderInfo);

            // sort OIT nodes

            renderInfo.setRenderProgram(m_->OITSortNodesRenderProgram());
            renderInfo.setFrameBuffer(camera->OITSortModesFrameBuffer());

            renderInfo.setDepthTest(false);
            renderInfo.setColorMasks(false);

            renderer->clearRenderData();
            renderer->addRenderData(glm::mat4x4(1.f), m_->screenQuadFinalDrawable());
            renderer->render(renderInfo);

            // render final
            renderInfo.setRenderProgram(m_->finalRenderProgram());
            renderInfo.setFrameBuffer(camera->finalFrameBuffer());
            renderInfo.setGBufferMaps(camera->GFrameBufferColorMap0(),
                                      camera->GFrameBufferColorMap1(),
                                      camera->GFrameBufferDepthStencilMap());

            renderInfo.setDepthTest(false);
            renderInfo.setColorMasks(true);

            renderer->clearRenderData();
            renderer->addRenderData(glm::mat4x4(1.f), m_->screenQuadFinalDrawable());
            renderer->render(renderInfo);

            renderer->blitFrameBuffer(camera->finalFrameBuffer(), renderer->defaultFrameBuffer(),
                                      0u, 0u, cameraViewportSize.x, cameraViewportSize.y,
                                      0u, 0u, screenSize.x, screenSize.y,
                                      true, false, false);
        }
    }

    renderer->clearRenderData();
}

std::shared_ptr<graphics::IRenderer> GraphicsEngine::graphicsRenderer() const
{
    return m_->renderer();
}

std::shared_ptr<RenderProgramsManager> GraphicsEngine::renderProgramsManager() const
{
    return m_->renderProgramsManager();
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
    auto drawable = std::dynamic_pointer_cast<ScreeQuadFinalDrawable>(m_->screenQuadFinalDrawable());
    assert(drawable);

    drawable->setRenderType(i);
}

}
}
