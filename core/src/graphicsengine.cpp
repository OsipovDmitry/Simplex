#include <algorithm>

#include <utils/logger.h>
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
#include <core/rendersurface.h>
#include <core/drawablenode.h>
#include <core/standarddrawable.h>
#include <core/nodevisitor.h>
#include <core/collectorvisitor.h>
#include <core/renderprogramsmanager.h>
#include <core/rendersurface.h>

#include "graphicsengineprivate.h"
#include "scenerootnodeprivate.h"
#include "nodeupdatevisitor.h"
#include "znearfarnodevisitor.h"
#include "drawablenoderenderer.h"


#include "drawablebaseprivate.h"
static int renderType = 0;

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
const std::string s_hasBaseColorMap = "HAS_BASE_COLOR_MAP";
const std::string s_hasMetalnessMap = "HAS_METALNESS_MAP";
const std::string s_hasRoughnessMap = "HAS_ROUGHNESS_MAP";
const std::string s_hasNormalMap = "HAS_NORMAL_MAP";

const std::filesystem::path s_deferredGeometryPassVertexShader = "D:/res/shaders/textured.vert";
const std::filesystem::path s_deferredGeometryPassFragmentShader = "D:/res/shaders/textured.frag";

const std::filesystem::path s_finalPassVertexShader = "D:/res/shaders/final.vert";
const std::filesystem::path s_finalPassFragmentShader = "D:/res/shaders/final.frag";

class DefaultRenderSurface : public RenderSurface
{
public:
    DefaultRenderSurface(std::shared_ptr<graphics::IRenderer> renderer)
        : RenderSurface(renderer, renderer->defaultFrameBuffer())
    {
    }
protected:
    void doResize(const glm::uvec2&) override
    {
    }
};

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    m_->renderer() = renderer;

    auto renderProgramsManager = std::make_shared<RenderProgramsManager>(renderer);
    m_->renderProgramsManager() = renderProgramsManager;

    auto defaultRenderSurface = std::make_shared<DefaultRenderSurface>(renderer);
    m_->defaultRenderSurface() = defaultRenderSurface;

    auto screenQuadMeshPainter = utils::MeshPainter(
                utils::Mesh::createEmptyMesh({{utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}}));
    screenQuadMeshPainter.drawScreenQuad();
    auto screenQuadVertexArray = renderer->createVertexArray(screenQuadMeshPainter.mesh());

    m_->screenQuadFinalDrawable() = std::make_shared<DrawableBase>(renderProgramsManager->loadOrGet(
                                                                       s_finalPassVertexShader,
                                                                       s_finalPassFragmentShader,
                                                                       {{s_vertexAttributesLocations[0], "0"}}),
                                                                   screenQuadVertexArray);

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
    auto renderProgram = m_->screenQuadFinalDrawable()->renderProgram();
    m_->screenQuadFinalDrawable()->m().uniforms()[renderProgram->uniformLocationByName("i")] = std::make_shared<Uniform<int32_t>>(renderType);


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

            camera->resizeRenderSurfaces(screenSize);

            auto gSurface = camera->gSurface();
            const auto &gSurfaceViewportSize = gSurface->viewportSize();
            float aspectRatio = static_cast<float>(gSurfaceViewportSize.x) / static_cast<float>(gSurfaceViewportSize.y);

            ZNearFarNodeVisitor zNearFarNodeVisitor(utils::OpenFrustum(camera->projectionMatrix(aspectRatio, 0.f, 1.f) *
                                                                       camera->globalTransform().inverted()));
            rootNode->accept(zNearFarNodeVisitor);
            auto zNear = glm::max(s_minZNear, zNearFarNodeVisitor.zNearFar()[0]);
            auto zFar = glm::min(s_maxZFar, zNearFarNodeVisitor.zNearFar()[1]);

            RenderInfo renderInfo;
            renderInfo.setViewMatrix(camera->globalTransform().inverted());
            renderInfo.setProjectionMatrix(camera->projectionMatrix(aspectRatio, zNear, zFar));
            renderInfo.setFrameBuffer(gSurface->frameBuffer());
            renderInfo.setViewport(glm::uvec4(0u, 0u, gSurfaceViewportSize));

            renderer->clearRenderData();
            if (false)
            {
                for (size_t j = 0; j < cameraNodeCollector.nodes().size(); ++j)
                {
                    if (i == j) continue;

                    const auto &anotherCamera = cameraNodeCollector.nodes()[j];
                    renderer->addRenderData(anotherCamera->globalTransform(), m_->cameraNodeCameraDrawable());
                }
            }
            DrawableNodeRenderer drawableNodeRenderer(renderer,
                                                      utils::Frustum(renderInfo.projectionMatrix() * renderInfo.viewMatrix()),
                                                      m_->nodeBoundingBoxDrawable(),
                                                      m_->drawableNodeLocalBoundingBoxDrawable());
            rootNode->accept(drawableNodeRenderer);
            gSurface->frameBuffer()->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::Color1});
            renderer->render(renderInfo);

            auto renderSurface = camera->renderSurface();

            renderInfo.setFrameBuffer(renderSurface->frameBuffer());
            renderInfo.setViewport(glm::uvec4(0u, 0u, renderSurface->viewportSize()));
            renderInfo.setGBufferMaps(gSurface->colorBuffer0(), gSurface->colorBuffer1(), gSurface->depthBuffer());
            renderInfo.setDepthTest(false);

            renderer->clearRenderData();
            renderer->addRenderData(glm::mat4x4(1.f), m_->screenQuadFinalDrawable());
            renderSurface->frameBuffer()->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});
            renderer->render(renderInfo);
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

std::shared_ptr<RenderSurface> GraphicsEngine::defaultRenderSurface() const
{
    return m_->defaultRenderSurface();
}

const std::vector<std::shared_ptr<Scene>> &GraphicsEngine::scenes() const
{
    return m_->scenes();
}

std::shared_ptr<Scene> GraphicsEngine::addNewScene(const std::string &name)
{
    auto scene = std::shared_ptr<Scene>(new Scene(name));
    scene->sceneRootNode()->m().scene() = scene;
    m_->scenes().push_back(scene);

    return scene;
}

void GraphicsEngine::removeScene(std::shared_ptr<Scene> scene)
{
    if (auto it = std::find(m_->scenes().begin(), m_->scenes().end(), scene); it != m_->scenes().end())
        m_->scenes().erase(it);
}

std::shared_ptr<StandardDrawable> GraphicsEngine::createStandardDrawable(std::shared_ptr<graphics::IVertexArray> vao,
                                                                         const glm::vec4& baseColor,
                                                                         float metalness,
                                                                         float roughness,
                                                                         std::shared_ptr<const graphics::ITexture> baseColorMap,
                                                                         std::shared_ptr<const graphics::ITexture> metalnessMap,
                                                                         std::shared_ptr<const graphics::ITexture> roughnessMap,
                                                                         std::shared_ptr<const graphics::ITexture> normalMap)
{
    utils::ShaderDefines defines;

    for (uint16_t i = 0; i < utils::numElementsVertexAttribute(); ++i)
    {
        if (vao->vertexAttributeBindingIndex(utils::castToVertexAttribute(i)) != static_cast<uint32_t>(-1))
            defines.insert({s_vertexAttributesLocations[i], std::to_string(i)});
    }

    if (baseColorMap)
        defines.insert({s_hasBaseColorMap, ""});

    if (metalnessMap)
        defines.insert({s_hasMetalnessMap, ""});

    if (roughnessMap)
        defines.insert({s_hasRoughnessMap, ""});

    if (normalMap)
        defines.insert({s_hasNormalMap, ""});

    auto renderProgram = m_->renderProgramsManager()->loadOrGet(s_deferredGeometryPassVertexShader,
                                                                s_deferredGeometryPassFragmentShader,
                                                                defines);

    return std::shared_ptr<StandardDrawable>(new StandardDrawable(
                                                 renderProgram, vao,
                                                 baseColor, metalness, roughness,
                                                 baseColorMap, metalnessMap, roughnessMap, normalMap
                                                 ));
}

void GraphicsEngine::setF(int i)
{
    renderType = i;
}

}
}
