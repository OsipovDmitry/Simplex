#include <algorithm>

#include <utils/logger.h>
#include <utils/meshpainter.h>

#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/background.h>
#include <core/cameranode.h>
#include <core/visualdrawablenode.h>
#include <core/lightnode.h>
#include <core/pbrdrawable.h>
#include <core/flatdrawable.h>
#include <core/nodecollector.h>
#include <core/shadow.h>
#include <core/renderinfo.h>
#include <core/settings.h>
#include <core/uniform.h>
#include <core/debuginformation.h>
#include <core/igraphicswidget.h>

#include "graphicsengineprivate.h"
#include "backgrounddrawable.h"
#include "cameranodeprivate.h"
#include "lightnodeprivate.h"
#include "backgroundprivate.h"
#include "shadowprivate.h"
#include "lightdrawable.h"
#include "blur.h"
#include "nodevisitorhelpers.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<graphics::RendererBase> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    static const auto &settings = settings::Settings::instance();
    static const auto &debugRenderSettings = settings.graphics().debugRendering();

    if (!renderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    m_->renderer() = renderer;
    m_->renderer()->makeCurrent();

    for (typename std::underlying_type<utils::VertexAttribute>::type i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (const auto &name = GraphicsEnginePrivate::vertexAttributeNameById(utils::castToVertexAttribute(i)); !name.empty())
            renderer->registerVertexAttribute(name, utils::castToVertexAttribute(i));
        else
            LOG_ERROR << "Attribute name is not set for id = " << i;

    for (typename std::underlying_type<UniformId>::type i = 1u; i < numElementsUniformId(); ++i) // 0u (undefined) is missed
        if (const auto &name = GraphicsEnginePrivate::uniformNameById(castToUniformId(i)); !name.empty())
            renderer->registerUniformId(name, i);
        else
            LOG_ERROR << "Uniform name is not set for id = " << i;

    for (typename std::underlying_type<SSBOId>::type i = 1u; i < numElementsSSBOId(); ++i) // 0u (undefined) is missed
        if (const auto &name = GraphicsEnginePrivate::SSBONameById(castToSSBOId(i)); !name.empty())
            renderer->registerSSBOId(name, i);
        else
            LOG_ERROR << "SSBO name is not set for id = " << i;

    m_->programsManager() = std::make_shared<ProgramsManager>(renderer);

    auto texturesManager = std::make_shared<TexturesManager>(renderer);
    m_->texturesManager() = texturesManager;

    const uint32_t maxFragments = settings.graphics().maxFragmants();
    const size_t fragmentsBufferSize = sizeof(maxFragments) + maxFragments * 24u; // 24 bytes per node
    auto fragmentsBuffer = renderer->createBuffer(fragmentsBufferSize);
    reinterpret_cast<uint32_t*>(fragmentsBuffer->map(graphics::IBuffer::MapAccess::WriteOnly, 0u, sizeof(uint32_t))->get())[0] = maxFragments;
    m_->fragmentsBuffer() = fragmentsBuffer;
    m_->fragmentsCounter() = renderer->createBuffer(sizeof(uint32_t));

    const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> screenQuadVertexDeclaration{
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}};
    m_->screenQuadVertexArray() = renderer->createVertexArray(utils::MeshPainter(utils::Mesh::createEmptyMesh(screenQuadVertexDeclaration)).drawScreenQuad().mesh());
    m_->screenQuadDrawable() = std::make_shared<Drawable>(m_->screenQuadVertexArray());

    const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> debugBoundingBoxVertexDeclaration{
        {utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}};

    utils::MeshPainter debugBoundingBoxPainter(utils::Mesh::createEmptyMesh(debugBoundingBoxVertexDeclaration));
    auto debugBoundingBoxVertexArray = renderer->createVertexArray(debugBoundingBoxPainter.drawBoundingBox().mesh());
    auto debugBoundingBoxBB = debugBoundingBoxPainter.calculateBoundingBox();

    m_->nodeBoundingBoxDrawable() = std::make_shared<FlatDrawable>(debugBoundingBoxVertexArray, debugBoundingBoxBB);
    m_->nodeBoundingBoxDrawable()->setColor(debugRenderSettings.nodeBoundingBox().color());

    m_->visualDrawableNodeLocalBoundingBoxDrawable() = std::make_shared<FlatDrawable>(debugBoundingBoxVertexArray, debugBoundingBoxBB);
    m_->visualDrawableNodeLocalBoundingBoxDrawable()->setColor(debugRenderSettings.visualDrawableNodeLocalBoundingBox().color());

    m_->visualDrawableBoundingBoxDrawable() = std::make_shared<FlatDrawable>(debugBoundingBoxVertexArray, debugBoundingBoxBB);
    m_->visualDrawableBoundingBoxDrawable()->setColor(debugRenderSettings.visualDrawableBoundingBox().color());

    m_->lightNodeAreaBoundingBoxDrawable() = std::make_shared<FlatDrawable>(debugBoundingBoxVertexArray, debugBoundingBoxBB);
    m_->lightNodeAreaBoundingBoxDrawable()->setColor(debugRenderSettings.lightNodeAreaBoundingBox().color());

    const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> lightAreaVertexDeclaration{
        {utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}};

    utils::MeshPainter pointLightAreaPainter(utils::Mesh::createEmptyMesh(lightAreaVertexDeclaration));
    m_->pointLightAreaVertexArray() = renderer->createVertexArray(pointLightAreaPainter.drawSphere(8u).mesh());
    m_->pointLightAreaBoundingBox() = pointLightAreaPainter.calculateBoundingBox();

    utils::MeshPainter spotLightAreaPainter(utils::Mesh::createEmptyMesh(lightAreaVertexDeclaration));
    m_->spotLightAreaVertexArray() = renderer->createVertexArray(spotLightAreaPainter.drawCone(8u).mesh());
    m_->spotLightAreaBoundingBox() = spotLightAreaPainter.calculateBoundingBox();

    utils::MeshPainter directionalLightAreaPainter(utils::Mesh::createEmptyMesh(lightAreaVertexDeclaration));
    m_->directionalLightAreaVertexArray() = renderer->createVertexArray(directionalLightAreaPainter.drawCube(glm::vec3(2.f)).mesh());


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

const std::string& GraphicsEngine::name() const
{
    return m_->name();
}

std::shared_ptr<IRenderer> GraphicsEngine::renderer()
{
    return graphicsRenderer();
}

std::shared_ptr<const IRenderer> GraphicsEngine::renderer() const
{
    return graphicsRenderer();
}

std::shared_ptr<graphics::RendererBase> GraphicsEngine::graphicsRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const graphics::RendererBase> GraphicsEngine::graphicsRenderer() const
{
    return const_cast<GraphicsEngine*>(this)->graphicsRenderer();
}

std::shared_ptr<TexturesManager> GraphicsEngine::texturesManager()
{
    return m_->texturesManager();
}

std::shared_ptr<const TexturesManager> GraphicsEngine::texturesManager() const
{
    return const_cast<GraphicsEngine*>(this)->texturesManager();
}

std::shared_ptr<ProgramsManager> GraphicsEngine::programsManager()
{
    return m_->programsManager();
}

std::shared_ptr<const ProgramsManager> GraphicsEngine::programsManager() const
{
    return const_cast<GraphicsEngine*>(this)->programsManager();
}

void GraphicsEngine::update(const std::shared_ptr<Scene>& scene,
    uint64_t /*time*/,
    uint32_t /*dt*/,
    debug::SceneInformation& sceneInfo)
{
    static const auto& settings = settings::Settings::instance();
    static const auto& debugRenderingSettings = settings.graphics().debugRendering();
    static const auto nodeLocalBoundingBoxFlag = debugRenderingSettings.nodeBoundingBox().isEnabled();
    static const auto visualDrawableNodeLocalBoundingBoxFlag = debugRenderingSettings.visualDrawableNodeLocalBoundingBox().isEnabled();
    static const auto visualDrawableBoundingBoxFlag = debugRenderingSettings.visualDrawableBoundingBox().isEnabled();
    static const auto lightNodeAreaBoundingBoxFlag = debugRenderingSettings.lightNodeAreaBoundingBox().isEnabled();
    static const auto debugRender =
        nodeLocalBoundingBoxFlag ||
        visualDrawableNodeLocalBoundingBoxFlag ||
        visualDrawableBoundingBoxFlag ||
        lightNodeAreaBoundingBoxFlag;

    if (!scene)
    {
        LOG_CRITICAL << "Scene can't be nullptr";
        return;
    }

    auto& renderer = m_->renderer();
    if (!renderer)
    {
        LOG_CRITICAL << "Graphics renderer can't be nullptr";
        return;
    }

    auto& programsManager = m_->programsManager();
    if (!programsManager)
    {
        LOG_CRITICAL << "Programs manager can't be nullptr";
        return;
    }

    renderer->makeCurrent();

    const auto& screenSize = renderer->screenSize();

    auto rootNode = scene->sceneRootNode();

    // render
    NodeCollector<CameraNode> cameraNodeCollector;
    rootNode->acceptDown(cameraNodeCollector);
    std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObjectComparator());

    for (const auto& camera : cameraNodeCollector.nodes())
    {
        auto& cameraPrivate = camera->m();

        debug::CameraInformation cameraInfo;
        cameraInfo.cameraName = camera->name();

        if (!camera->isRenderingEnabled())
            continue;

        cameraPrivate.resize(renderer, screenSize);

        const auto& cameraViewportSize = cameraPrivate.gFrameBuffer()->viewportSize();
        const auto cameraViewTransform = camera->globalTransform().inverted();

        ZRangeCalculator cameraZRangeCalculator(cameraViewTransform,
            camera->clipSpace(),
            camera->cullPlanesLimits(),
            true, true);
        rootNode->acceptDown(cameraZRangeCalculator);

        const utils::Frustum cameraFrustum(cameraViewTransform, camera->clipSpace(), cameraZRangeCalculator.resolveZRange());
        const auto& cameraProjectionMatrix = cameraFrustum.projectionMatrix();

        VisualDrawablesCollector cameraVisualDrawablesCollector(cameraFrustum);
        rootNode->acceptDown(cameraVisualDrawablesCollector);

        LightNodesCollector lightNodesCollector(cameraFrustum);
        rootNode->acceptDown(lightNodesCollector);

        if (debugRender)
        {
            DebugGeometryCollector cameraDebugGeometryCollector(cameraFrustum,
                nodeLocalBoundingBoxFlag,
                visualDrawableNodeLocalBoundingBoxFlag,
                visualDrawableBoundingBoxFlag,
                lightNodeAreaBoundingBoxFlag);
            rootNode->acceptDown(cameraDebugGeometryCollector);

            auto& cameraVisualDrawables = cameraVisualDrawablesCollector.visualDrawables();

            for (const auto& modelMatrix : cameraDebugGeometryCollector.nodeBoundingBoxes())
                cameraVisualDrawables.push_back(std::make_tuple(m_->nodeBoundingBoxDrawable(), modelMatrix));

            for (const auto& modelMatrix : cameraDebugGeometryCollector.visualDrawableNodeLocalBoundingBoxes())
                cameraVisualDrawables.push_back(std::make_tuple(m_->visualDrawableNodeLocalBoundingBoxDrawable(), modelMatrix));

            for (const auto& modelMatrix : cameraDebugGeometryCollector.visualDrawableBoundingBoxes())
                cameraVisualDrawables.push_back(std::make_tuple(m_->visualDrawableBoundingBoxDrawable(), modelMatrix));

            for (const auto& modelMatrix : cameraDebugGeometryCollector.lightNodeAreaBoundingBoxes())
                cameraVisualDrawables.push_back(std::make_tuple(m_->lightNodeAreaBoundingBoxDrawable(), modelMatrix));
        }

        auto cameraRenderInfo = std::make_shared<RenderInfo>();
        cameraRenderInfo->setGBuffer(cameraPrivate.gFrameBuffer()->colorTexture(),
            cameraPrivate.gFrameBuffer()->depthTexture(),
            cameraPrivate.gFrameBuffer()->oitDepthImage(),
            cameraPrivate.gFrameBuffer()->oitIndicesImage(),
            m_->fragmentsBuffer(),
            m_->fragmentsCounter());

        // reset fragments counter buffer
        auto &fragmentsCounterBuffer = m_->fragmentsCounter();
        if (auto fragmentsCounterBufferData = fragmentsCounterBuffer->map(graphics::IBuffer::MapAccess::WriteOnly); fragmentsCounterBuffer)
            *reinterpret_cast<uint32_t*>(fragmentsCounterBufferData->get()) = 0u;

        // clear indices image
        renderer->compute(programsManager->loadOrGetOITClearPassComputeProgram(),
            glm::uvec3(cameraPrivate.gFrameBuffer()->viewportSize(), 1u),
            cameraRenderInfo);

        // render opaque geometries
        cameraPrivate.gFrameBuffer()->setForGeometryOpaquePass();
        renderer->clearRenderData();
        for (const auto& [visualDrawable, modelMatrix] : cameraVisualDrawablesCollector.visualDrawables())
            if (!visualDrawable->isTransparent())
            {
                renderer->addRenderData(programsManager->loadOrGetGeometryPassRenderProgram(
                    visualDrawable->vertexAttrubiteSet(),
                    visualDrawable->uniformCollection(),
                    false),
                    visualDrawable,
                    modelMatrix);
                ++cameraInfo.numOpaqueDrawablesRendered;
            }
        renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
            glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
            cameraViewTransform,
            cameraProjectionMatrix,
            cameraRenderInfo);

        // render transparent geometries
        cameraPrivate.gFrameBuffer()->setForGeometryTransparentPass();
        renderer->clearRenderData();
        for (const auto& [visualDrawable, modelMatrix] : cameraVisualDrawablesCollector.visualDrawables())
            if (visualDrawable->isTransparent())
            {
                renderer->addRenderData(programsManager->loadOrGetGeometryPassRenderProgram(
                    visualDrawable->vertexAttrubiteSet(),
                    visualDrawable->uniformCollection(),
                    true),
                    visualDrawable,
                    modelMatrix);
                ++cameraInfo.numTransparentDrawablesRendered;
            }
        renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
            glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
            cameraViewTransform,
            cameraProjectionMatrix,
            cameraRenderInfo);

        // get number of fragments rendered
        if (auto nodesCounterBufferData = fragmentsCounterBuffer->map(graphics::IBuffer::MapAccess::ReadOnly); nodesCounterBufferData)
            cameraInfo.numFragmentsRendered = *reinterpret_cast<uint32_t*>(nodesCounterBufferData->get());

        // sort fragments
        renderer->compute(programsManager->loadOrGetOITSortNodesPassComputeProgram(), glm::uvec3(cameraViewportSize, 1u), cameraRenderInfo);

        // render background layer (it's here because next stencil buffer will be reused for light-areas rendering)
        cameraPrivate.gFrameBuffer()->setForBackgroundPass();
        renderer->clearRenderData();
        auto& backgroundDrawable = scene->background().m().drawable();
        renderer->addRenderData(programsManager->loadOrGetBackgroundPassRenderProgram(
            backgroundDrawable->vertexAttrubiteSet(),
            backgroundDrawable->uniformCollection()),
            backgroundDrawable);
        renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
            glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
            cameraViewTransform,
            cameraProjectionMatrix,
            cameraRenderInfo);

        // render SSAO
        auto& ssao = camera->ssao();
        //        if (ssao.mode() != SSAOMode::Disabled)
        //        {
        //            auto &ssaoPrivate = ssao.m();
        //            auto &ssaoDrawable = ssaoPrivate.drawable();
        //            renderer->clearRenderData();
        //            renderer->addRenderData(programsManager->loadOrGetSSAORenderProgram(ssaoDrawable->vertexAttrubiteSet(), ssao.mode()),
        //                                    ssaoDrawable);
        //            renderer->render(cameraPrivate.ssaoFrameBuffer()->frameBuffer(),
        //                             glm::uvec4(0u, 0u, cameraPrivate.ssaoFrameBuffer()->viewportSize()),
        //                             cameraViewTransform,
        //                             cameraProjectionMatrix,
        //                             cameraRenderInfo);

        ////            if (true)
        ////            {
        ////                auto &blurDrawable = ssaoPrivate.blur();
        ////                blurDrawable->setSourceDepthMap(cameraPrivate.gFrameBuffer()->depthTexture());
        ////                //blurDrawable->setMaxDepthDifference(ssao->radius());

        ////                blurDrawable->setHorizontalDirection(true);
        ////                blurDrawable->setSourceMap(cameraPrivate.ssaoFrameBuffer()->colorTexture());
        ////                renderer->clearRenderData();
        ////                renderer->addRenderData(programsManager->loadOrGetBilaterialBlurRenderProgram(blurDrawable->vertexAttrubiteSet()), blurDrawable);
        ////                renderer->render(cameraPrivate.ssaoBlurFrameBuffer()->frameBuffer(),
        ////                                 glm::uvec4(0u, 0u, cameraPrivate.ssaoBlurFrameBuffer()->viewportSize()),
        ////                                 cameraRenderInfo);

        ////                blurDrawable->setHorizontalDirection(false);
        ////                blurDrawable->setSourceMap(cameraPrivate.ssaoBlurFrameBuffer()->colorTexture());
        ////                renderer->clearRenderData();
        ////                renderer->addRenderData(programsManager->loadOrGetBilaterialBlurRenderProgram(blurDrawable->vertexAttrubiteSet()), blurDrawable);
        ////                renderer->render(cameraPrivate.ssaoFrameBuffer()->frameBuffer(),
        ////                                 glm::uvec4(0u, 0u, cameraPrivate.ssaoFrameBuffer()->viewportSize()),
        ////                                 cameraRenderInfo);
        ////            }

        //            cameraRenderInfo->setSSAOMap(cameraPrivate.ssaoFrameBuffer()->colorTexture());
        //        }

        //        // clear light color buffer
        //        cameraPrivate.lightFrameBuffer()->setForClearPass();
        //        renderer->clearRenderData();
        //        renderer->render(cameraPrivate.lightFrameBuffer()->frameBuffer(),
        //                         glm::uvec4(0u, 0u, cameraPrivate.lightFrameBuffer()->viewportSize()),
        //                         cameraViewTransform,
        //                         cameraProjectionMatrix,
        //                         cameraRenderInfo);

                // render lights areas
        for (const auto& lightNode : lightNodesCollector.lightNodes())
        {
            if (!lightNode->isLightingEnabled())
                continue;

            auto& lightPrivate = lightNode->m();
            auto& shadow = lightNode->shadow();
            auto shadingMode = shadow.mode();
            auto shadingFilter = shadow.filter();

            // render shadows
            if (shadingMode != ShadingMode::Disabled)
            {
                auto& shadowPrivate = shadow.m();

                const auto& shadowTransform = lightPrivate.shadowTransform(cameraFrustum.points());

                ZRangeCalculator shadowZRangeCalculator(shadowTransform.frustumViewTransform,
                    shadowTransform.frustumClipSpace,
                    shadowTransform.cullPlanesLimits,
                    true, false);
                rootNode->acceptDown(shadowZRangeCalculator);

                auto shadowZRange = shadowZRangeCalculator.resolveZRange();
                const utils::Frustum shadowFrustum(shadowTransform.frustumViewTransform,
                    shadowTransform.frustumClipSpace,
                    shadowZRange);

                VisualDrawablesCollector shadowVisualDrawablesCollector(shadowFrustum);
                rootNode->acceptDown(shadowVisualDrawablesCollector);

                const auto shadowProjectionMatrix = shadowTransform.clipSpase.projectionMatrix(shadowZRange);
                std::vector<glm::mat4x4> layeredShadowMatrices(shadowTransform.layeredViewTransforms.size());
                for (size_t i = 0u; i < shadowTransform.layeredViewTransforms.size(); ++i)
                    layeredShadowMatrices[i] = shadowProjectionMatrix * shadowTransform.layeredViewTransforms[i];
                shadowPrivate.update(renderer, layeredShadowMatrices);

                auto shadowRenderInfo = std::make_shared<RenderInfo>();
                shadowRenderInfo->setLayeredShadowMatricesBuffer(shadowPrivate.layeredMatricesBuffer());

                auto& shadowFrameBuffer = shadowPrivate.frameBuffer();
                const auto shadowViewport = glm::uvec4(0u, 0u, shadowPrivate.mapSize());

                // render opaque shadow
                shadowFrameBuffer->setForOpaquePass();
                renderer->clearRenderData();
                for (const auto& [visualDrawable, modelMatrix] : shadowVisualDrawablesCollector.visualDrawables())
                {
                    if (!visualDrawable->isTransparent() || (shadingMode == ShadingMode::OpaqueAndTransparent))
                        renderer->addRenderData(programsManager->loadOrGetShadowRenderProgram(
                            visualDrawable->vertexAttrubiteSet(),
                            visualDrawable->uniformCollection(),
                            shadingMode,
                            shadingFilter),
                            visualDrawable,
                            modelMatrix);
                }
                renderer->render(shadowFrameBuffer->frameBuffer(),
                    shadowViewport,
                    glm::mat4x4(1.f),
                    glm::mat4x4(1.f),
                    shadowRenderInfo);

                // render transparent shadow
                if (shadingMode == ShadingMode::Color)
                {
                    shadowFrameBuffer->setForTransparentPass();
                    renderer->clearRenderData();
                    for (const auto& [visualDrawable, modelMatrix] : shadowVisualDrawablesCollector.visualDrawables())
                    {
                        if (visualDrawable->isTransparent())
                            renderer->addRenderData(programsManager->loadOrGetShadowRenderProgram(
                                visualDrawable->vertexAttrubiteSet(),
                                visualDrawable->uniformCollection(),
                                shadingMode,
                                shadingFilter),
                                visualDrawable,
                                modelMatrix);
                    }
                    renderer->render(shadowFrameBuffer->frameBuffer(),
                        shadowViewport,
                        glm::mat4x4(1.f),
                        glm::mat4x4(1.f),
                        shadowRenderInfo);
                }

                // update light drawable
                auto shadowDepthMap = shadowFrameBuffer->depthTexture();
                auto shadowColorMap = shadowFrameBuffer->colorTexture();

                if (shadingFilter == ShadingFilter::VSM)
                {
                    auto& blur = shadowPrivate.blur();
                    blur->setSourceMap(shadowFrameBuffer->depthVSMTexture(), 0u, true);
                    shadowDepthMap = blur->run(renderer, m_->programsManager());
                }

                static const auto s_biasTransform = utils::Transform::makeTranslation(glm::vec3(.5f)) * utils::Transform::makeScale(.5f);
                auto& lightDrawable = lightPrivate.areaDrawable();
                lightDrawable->setShadowDepthMap(shadowDepthMap);
                lightDrawable->setShadowColorMap(shadowColorMap);
                lightDrawable->setShadowViewMatrix(shadowTransform.frustumViewTransform);
                lightDrawable->setShadowProjectionMatrix(s_biasTransform * shadowProjectionMatrix);
                lightDrawable->setShadowDepthBias(shadow.depthBias());
            }

            auto lightType = lightNode->type();
            auto modelMatrix = lightNode->globalTransform() * lightPrivate.areaMatrix();
            auto& lightDrawable = lightPrivate.areaDrawable();
            auto attributesSet = lightDrawable->vertexAttrubiteSet();

            // stencil pass
            cameraPrivate.gFrameBuffer()->setForStencilPass();
            renderer->clearRenderData();
            renderer->addRenderData(programsManager->loadOrGetStencilPassRenderProgram(attributesSet,
                lightType),
                lightDrawable,
                modelMatrix);
            renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
                glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
                cameraViewTransform,
                cameraProjectionMatrix,
                cameraRenderInfo);

            // light pass
            cameraPrivate.gFrameBuffer()->setForLightPass();
            renderer->clearRenderData();
            renderer->addRenderData(programsManager->loadOrGetLightPassRenderProgram(attributesSet,
                lightDrawable->uniformCollection(),
                lightType,
                shadingMode,
                shadingFilter,
                ssao.mode()),
                lightDrawable,
                modelMatrix);
            renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
                glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
                cameraViewTransform,
                cameraProjectionMatrix,
                cameraRenderInfo);

            ++cameraInfo.numLightsRendered;
        }

        // render final layer
        cameraPrivate.gFrameBuffer()->setForFinalPass();
        renderer->clearRenderData();
        renderer->addRenderData(programsManager->loadOrGetFinalPassRenderProgram(
            m_->screenQuadDrawable()->vertexAttrubiteSet()),
            m_->screenQuadDrawable());
        renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
            glm::uvec4(0u, 0u, cameraPrivate.gFrameBuffer()->viewportSize()),
            cameraViewTransform,
            cameraProjectionMatrix,
            cameraRenderInfo);

        // postprocess
        renderer->clearRenderData();
        renderer->addRenderData(programsManager->loadOrGetPostprocessPassRenderProgram(
            m_->screenQuadDrawable()->vertexAttrubiteSet()),
            m_->screenQuadDrawable());
        renderer->render(cameraPrivate.postprocessFrameBuffer()->frameBuffer(),
            glm::uvec4(0u, 0u, cameraPrivate.postprocessFrameBuffer()->viewportSize()),
            cameraViewTransform,
            cameraProjectionMatrix,
            cameraRenderInfo);

        sceneInfo.camerasInformation.push_back(cameraInfo);

        if (b)
        {
            //
            b = false;
        }

        if (upShading)
        {
            upShading = false;
            for (auto lightNode : lightNodesCollector.lightNodes())
            {
                if (lightNode->asIBLLightNode())
                    continue;

                lightNode->shadow().setMode(tempMode);
                lightNode->shadow().setFilter(tempFilter);
            }
        }

        //        renderer->blitFrameBuffer(
        //                    lightNodesCollector.lightNodes().begin()->get()->shadow().m().frameBuffer()->frameBuffer(),
        //                    cameraPrivate.postprocessFrameBuffer()->frameBuffer(),
        //                    glm::uvec4(0u, 0u, lightNodesCollector.lightNodes().begin()->get()->shadow().mapSize()),
        //                    glm::uvec4(0u, 0u, lightNodesCollector.lightNodes().begin()->get()->shadow().mapSize()),
        //                    true, false, false, false);
    }

    renderer->clearRenderData();
}


void GraphicsEngine::setF(int i)
{
    if (i == -1)
    {
        b = true;
    }
}

}
}
