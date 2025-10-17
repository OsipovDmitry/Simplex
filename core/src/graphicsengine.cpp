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
#include <core/drawablenode.h>
#include <core/lightnode.h>
#include <core/nodecollector.h>
#include <core/shadow.h>
#include <core/settings.h>
#include <core/uniform.h>
#include <core/debuginformation.h>
#include <core/igraphicswidget.h>

#include "graphicsengineprivate.h"
#include "cameranodeprivate.h"
#include "lightnodeprivate.h"
#include "backgroundprivate.h"
#include "shadowprivate.h"
#include "blur.h"
#include "nodevisitorhelpers.h"
#include "framebufferhelpers.h"
#include "renderpasshelpers.h"
#include "sceneprivate.h"
#include "scenedata.h"
#include "geometrybuffer.h"
#include "shadowbuffer.h"
#include "geometryrenderinfo.h"
#include "shadowrenderinfo.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, const std::shared_ptr<graphics::RendererBase>& renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    static const auto &settings = settings::Settings::instance();
    static const auto &debugRenderSettings = settings.graphics().debugRendering();

    if (!renderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    m_->renderer() = renderer;
    m_->renderer()->makeCurrent();

    for (typename std::underlying_type<utils::VertexAttribute>::type i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (const auto &name = GraphicsEnginePrivate::attributeNameByID(utils::castToVertexAttribute(i)); !name.empty())
            renderer->registerAttribute(name, utils::castToVertexAttribute(i));
        else
            LOG_ERROR << "Attribute name has not been set for ID = " << i;

    for (typename std::underlying_type<graphics::FrameBufferAttachment>::type i = 0u;
        i < graphics::FrameBufferColorAttachmentsCount();
        ++i)
        if (const auto& name = GraphicsEnginePrivate::outputNameByID(graphics::FrameBufferColorAttachment(i)); !name.empty())
            renderer->registerOutput(name, graphics::castToFrameBufferAttachment(i));
        else
            LOG_ERROR << "Output name has not been set for ID = " << i;

    for (typename std::underlying_type<UniformID>::type i = 0u; i < numElementsUniformID(); ++i)
        if (const auto &name = GraphicsEnginePrivate::uniformNameByID(castToUniformID(i)); !name.empty())
            renderer->registerUniform(name, core::castToUniformID(i));
        else
            LOG_ERROR << "Uniform name has not been set for ID = " << i;

    for (typename std::underlying_type<UniformBlockID>::type i = 0u; i < numElementsUniformBlockID(); ++i)
        if (const auto& name = GraphicsEnginePrivate::uniformBlockNameByID(castToUniformBlockID(i)); !name.empty())
            renderer->registerUniformBlock(name, core::castToUniformBlockID(i));
        else
            LOG_ERROR << "Uniform block name has not been set for ID = " << i;

    for (typename std::underlying_type<ShaderStorageBlockID>::type i = 0u; i < numElementsShaderStorageBlockID(); ++i)
        if (const auto &name = GraphicsEnginePrivate::shaderStorageBlockNameByID(castToShaderStorageBlockID(i)); !name.empty())
            renderer->registerShaderStorageBlock(name, core::castToShaderStorageBlockID(i));
        else
            LOG_ERROR << "Shader storage block name has not been set for ID = " << i;

    m_->programsManager() = std::make_shared<ProgramsManager>(renderer);

    auto texturesManager = std::make_shared<TexturesManager>(renderer);
    m_->texturesManager() = texturesManager;

    m_->frameBuffer() = renderer->createFrameBuffer();
    m_->vertexArray() = renderer->createVertexArray();
    m_->geometryBuffer() = std::make_shared<GeometryBuffer>(glm::uvec2(0u));

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

std::shared_ptr<core::Scene> GraphicsEngine::scene()
{
    return m_->scene();
}

std::shared_ptr<const core::Scene> GraphicsEngine::scene() const
{
    return const_cast<GraphicsEngine*>(this)->scene();
}

void GraphicsEngine::setScene(const std::shared_ptr<core::Scene>& value)
{
    m_->scene() = value;

    auto& passes = m_->passes();
    auto& programsManager = m_->programsManager();
    passes.clear();

    auto buildDrawDataCommandsBufferPass = std::make_shared<BuildDrawDataCommandsBufferPass>(programsManager);
    passes.push_back(buildDrawDataCommandsBufferPass);

    auto renderDrawDataGeometryPass = std::make_shared<RenderDrawDataGeometryPass>(
        programsManager,
        buildDrawDataCommandsBufferPass->opaqueCommandsBuffer(),
        buildDrawDataCommandsBufferPass->transparentCommandsBuffer());
    passes.push_back(renderDrawDataGeometryPass);

    auto renderBackgroundPass = std::make_shared<RenderBackgroundPass>(programsManager);
    passes.push_back(renderBackgroundPass);

    auto buildLightsCommandsBufferPass = std::make_shared<BuildLightsCommandsBufferPass>(programsManager);
    passes.push_back(buildLightsCommandsBufferPass);

    auto renderDrawDataStencilPass = std::make_shared<RenderDrawDataStencilPass>(
        programsManager,
        buildLightsCommandsBufferPass->lightsCommandsBuffer());
    passes.push_back(renderDrawDataStencilPass);

    auto blendPass = std::make_shared<BlendPass>(programsManager);
    passes.push_back(blendPass);
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

void GraphicsEngine::update(uint64_t /*time*/, uint32_t /*dt*/, debug::SceneInformation& sceneInfo)
{
    static const auto& settings = settings::Settings::instance();
    static const auto& debugRenderingSettings = settings.graphics().debugRendering();
    static const auto nodeLocalBoundingBoxFlag = debugRenderingSettings.nodeBoundingBox().isEnabled();
    static const auto visualDrawableNodeLocalBoundingBoxFlag = debugRenderingSettings.drawableNodeLocalBoundingBox().isEnabled();
    static const auto visualDrawableBoundingBoxFlag = debugRenderingSettings.drawableBoundingBox().isEnabled();
    static const auto lightNodeAreaBoundingBoxFlag = debugRenderingSettings.lightNodeAreaBoundingBox().isEnabled();
    static const auto debugRender =
        nodeLocalBoundingBoxFlag ||
        visualDrawableNodeLocalBoundingBoxFlag ||
        visualDrawableBoundingBoxFlag ||
        lightNodeAreaBoundingBoxFlag;

    auto scene = GraphicsEngine::scene();
    if (!scene)
        return;

    auto renderer = graphicsRenderer();
    if (!renderer)
    {
        LOG_CRITICAL << "Graphics renderer can't be nullptr";
        return;
    }

    auto programsManager = GraphicsEngine::programsManager();
    if (!programsManager)
    {
        LOG_CRITICAL << "Programs manager can't be nullptr";
        return;
    }

    renderer->makeCurrent();

    auto widget = renderer->widget();
    if (!widget)
    {
        LOG_CRITICAL << "Graphics widget can't be nullptr";
        return;
    }

    const auto screenSize = widget->size();
    m_->geometryBuffer()->resize(screenSize);

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

        std::shared_ptr<GeometryBuffer> cameraGeometryBuffer = m_->geometryBuffer();
        if (!camera->isDefaultFramebufferUsed())
            cameraGeometryBuffer = cameraPrivate.geometryBuffer();

        cameraPrivate.resize(cameraGeometryBuffer->size());

        const auto& cameraViewportSize = cameraGeometryBuffer->size();
        const auto cameraViewTransform = camera->globalTransform().inverted();

        ZRangeCalculator cameraZRangeCalculator(cameraViewTransform,
            camera->clipSpace(),
            camera->cullPlanesLimits(),
            true, true);
        rootNode->acceptDown(cameraZRangeCalculator);

        const utils::Frustum cameraFrustum(cameraViewTransform, camera->clipSpace(), cameraZRangeCalculator.resolveZRange());
        const auto& cameraProjectionMatrix = cameraFrustum.projectionMatrix();

        DrawablesCollector cameraVisualDrawablesCollector(cameraFrustum);
        rootNode->acceptDown(cameraVisualDrawablesCollector);

        LightNodesCollector lightNodesCollector(cameraFrustum);
        rootNode->acceptDown(lightNodesCollector);

        if (debugRender)
        {
            //DebugGeometryCollector cameraDebugGeometryCollector(cameraFrustum,
            //    nodeLocalBoundingBoxFlag,
            //    visualDrawableNodeLocalBoundingBoxFlag,
            //    visualDrawableBoundingBoxFlag,
            //    lightNodeAreaBoundingBoxFlag);
            //rootNode->acceptDown(cameraDebugGeometryCollector);

            //auto& cameraVisualDrawables = cameraVisualDrawablesCollector.drawables();

            //for (const auto& modelMatrix : cameraDebugGeometryCollector.nodeBoundingBoxes())
            //    cameraVisualDrawables.push_back(std::make_tuple(m_->nodeBoundingBoxDrawable(), modelMatrix));

            //for (const auto& modelMatrix : cameraDebugGeometryCollector.drawableNodeLocalBoundingBoxes())
            //    cameraVisualDrawables.push_back(std::make_tuple(m_->drawableNodeLocalBoundingBoxDrawable(), modelMatrix));

            //for (const auto& modelMatrix : cameraDebugGeometryCollector.drawableBoundingBoxes())
            //    cameraVisualDrawables.push_back(std::make_tuple(m_->drawableBoundingBoxDrawable(), modelMatrix));

            //for (const auto& modelMatrix : cameraDebugGeometryCollector.lightNodeAreaBoundingBoxes())
            //    cameraVisualDrawables.push_back(std::make_tuple(m_->lightNodeAreaBoundingBoxDrawable(), modelMatrix));
        }

        auto cameraRenderInfo = std::make_shared<GeometryRenderInfo>();
        cameraRenderInfo->setViewMatrix(cameraViewTransform);
        cameraRenderInfo->setProjectionMatrix(cameraProjectionMatrix);
        cameraRenderInfo->setGeometryBuffer(cameraGeometryBuffer);

        auto& sceneData = scene->m().sceneData();

        for (auto& pass : m_->passes())
            pass->run(renderer, m_->frameBuffer(), m_->vertexArray(), cameraRenderInfo, sceneData);

        //// reset fragments counter buffer
        //cameraGeometryBuffer->clearOITNodesBuffer();

        //// clear indices image
        //renderer->compute(programsManager->loadOrGetOITClearPassComputeProgram(),
        //    glm::uvec3(cameraGeometryBuffer->size(), 1u),
        //    { cameraRenderInfo });

        //// render opaque geometries
        //cameraPrivate.gFrameBuffer()->setForGeometryOpaquePass();
        //renderer->clearRenderData();
        //for (const auto& [visualDrawable, modelMatrix] : cameraVisualDrawablesCollector.visualDrawables())
        //    if (!visualDrawable->isTransparent())
        //    {
        //        renderer->addRenderData(programsManager->loadOrGetGeometryPassRenderProgram(
        //            visualDrawable->vertexAttrubiteSet(),
        //            visualDrawable->uniformCollection(),
        //            false),
        //            visualDrawable,
        //            modelMatrix);
        //        ++cameraInfo.numOpaqueDrawablesRendered;
        //    }
        //renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //    glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //    cameraRenderInfo);

        //// render transparent geometries
        //cameraPrivate.gFrameBuffer()->setForGeometryTransparentPass();
        //renderer->clearRenderData();
        //for (const auto& [visualDrawable, modelMatrix] : cameraVisualDrawablesCollector.visualDrawables())
        //    if (visualDrawable->isTransparent())
        //    {
        //        renderer->addRenderData(programsManager->loadOrGetGeometryPassRenderProgram(
        //            visualDrawable->vertexAttrubiteSet(),
        //            visualDrawable->uniformCollection(),
        //            true),
        //            visualDrawable,
        //            modelMatrix);
        //        ++cameraInfo.numTransparentDrawablesRendered;
        //    }
        //renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //    glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //    cameraRenderInfo);

        //// get number of fragments rendered
        //cameraInfo.numFragmentsRendered = cameraGeometryBuffer->OITNodesBuffer()->reservedData().numOITNodes;

        //// sort fragments
        //renderer->compute(
        //    programsManager->loadOrGetOITSortNodesPassComputeProgram(),
        //    glm::uvec3(cameraViewportSize, 1u),
        //    { cameraRenderInfo });

        //// render background layer (it's here because next stencil buffer will be reused for light-areas rendering)
        //cameraPrivate.gFrameBuffer()->setForBackgroundPass();
        //renderer->clearRenderData();
        //auto& backgroundDrawable = scene->background()->m().drawable();
        //renderer->addRenderData(programsManager->loadOrGetBackgroundPassRenderProgram(
        //    backgroundDrawable->vertexAttrubiteSet(),
        //    backgroundDrawable->uniformCollection()),
        //    backgroundDrawable);
        //renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //    glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //    cameraRenderInfo);

        //// render SSAO
        //auto& ssao = camera->ssao();
        ////        if (ssao.mode() != SSAOMode::Disabled)
        ////        {
        ////            auto &ssaoPrivate = ssao.m();
        ////            auto &ssaoDrawable = ssaoPrivate.drawable();
        ////            renderer->clearRenderData();
        ////            renderer->addRenderData(programsManager->loadOrGetSSAORenderProgram(ssaoDrawable->vertexAttrubiteSet(), ssao.mode()),
        ////                                    ssaoDrawable);
        ////            renderer->render(cameraPrivate.ssaoFrameBuffer()->frameBuffer(),
        ////                             glm::uvec4(0u, 0u, cameraPrivate.ssaoFrameBuffer()->viewportSize()),
        ////                             cameraViewTransform,
        ////                             cameraProjectionMatrix,
        ////                             cameraRenderInfo);

        //////            if (true)
        //////            {
        //////                auto &blurDrawable = ssaoPrivate.blur();
        //////                blurDrawable->setSourceDepthMap(cameraPrivate.gFrameBuffer()->depthTexture());
        //////                //blurDrawable->setMaxDepthDifference(ssao->radius());

        //////                blurDrawable->setHorizontalDirection(true);
        //////                blurDrawable->setSourceMap(cameraPrivate.ssaoFrameBuffer()->colorTexture());
        //////                renderer->clearRenderData();
        //////                renderer->addRenderData(programsManager->loadOrGetBilaterialBlurRenderProgram(blurDrawable->vertexAttrubiteSet()), blurDrawable);
        //////                renderer->render(cameraPrivate.ssaoBlurFrameBuffer()->frameBuffer(),
        //////                                 glm::uvec4(0u, 0u, cameraPrivate.ssaoBlurFrameBuffer()->viewportSize()),
        //////                                 cameraRenderInfo);

        //////                blurDrawable->setHorizontalDirection(false);
        //////                blurDrawable->setSourceMap(cameraPrivate.ssaoBlurFrameBuffer()->colorTexture());
        //////                renderer->clearRenderData();
        //////                renderer->addRenderData(programsManager->loadOrGetBilaterialBlurRenderProgram(blurDrawable->vertexAttrubiteSet()), blurDrawable);
        //////                renderer->render(cameraPrivate.ssaoFrameBuffer()->frameBuffer(),
        //////                                 glm::uvec4(0u, 0u, cameraPrivate.ssaoFrameBuffer()->viewportSize()),
        //////                                 cameraRenderInfo);
        //////            }

        ////            cameraRenderInfo->setSSAOMap(cameraPrivate.ssaoFrameBuffer()->colorTexture());
        ////        }

        ////        // clear light color buffer
        ////        cameraPrivate.lightFrameBuffer()->setForClearPass();
        ////        renderer->clearRenderData();
        ////        renderer->render(cameraPrivate.lightFrameBuffer()->frameBuffer(),
        ////                         glm::uvec4(0u, 0u, cameraPrivate.lightFrameBuffer()->viewportSize()),
        ////                         cameraViewTransform,
        ////                         cameraProjectionMatrix,
        ////                         cameraRenderInfo);

        //// render lights areas
        //for (const auto& lightNode : lightNodesCollector.lightNodes())
        //{
        //    if (!lightNode->isLightingEnabled())
        //        continue;

        //    auto& lightPrivate = lightNode->m();
        //    auto& shadow = lightNode->shadow();
        //    const auto shadingMode = shadow.mode();
        //    const auto shadingFilter = shadow.filter();

        //    // render shadows
        //    if (shadingMode != ShadingMode::Disabled)
        //    {
        //        auto& shadowPrivate = shadow.m();

        //        const auto& shadowTransform = lightPrivate.shadowTransform(cameraFrustum.points());

        //        ZRangeCalculator shadowZRangeCalculator(shadowTransform.frustumViewTransform,
        //            shadowTransform.frustumClipSpace,
        //            shadowTransform.cullPlanesLimits,
        //            true, false);
        //        rootNode->acceptDown(shadowZRangeCalculator);

        //        auto shadowZRange = shadowZRangeCalculator.resolveZRange();
        //        const utils::Frustum shadowFrustum(shadowTransform.frustumViewTransform,
        //            shadowTransform.frustumClipSpace,
        //            shadowZRange);

        //        VisualDrawablesCollector shadowVisualDrawablesCollector(shadowFrustum);
        //        rootNode->acceptDown(shadowVisualDrawablesCollector);

        //        const auto shadowProjectionMatrix = shadowTransform.clipSpase.projectionMatrix(shadowZRange);
        //        std::vector<glm::mat4x4> layeredShadowMatrices(shadowTransform.layeredViewTransforms.size());
        //        for (size_t i = 0u; i < shadowTransform.layeredViewTransforms.size(); ++i)
        //            layeredShadowMatrices[i] = shadowProjectionMatrix * shadowTransform.layeredViewTransforms[i];
        //        shadowPrivate.update(m_->frameBuffer(), layeredShadowMatrices);

        //        auto shadowRenderInfo = std::make_shared<ShadowRenderInfo>();
        //        shadowRenderInfo->setViewportSize(shadowPrivate.mapSize());
        //        shadowRenderInfo->setLayeredShadowMatricesBuffer(shadowPrivate.layeredMatricesBuffer()->buffer());

        //        auto& shadowFrameBuffer = shadowPrivate.frameBuffer();
        //        const auto shadowViewport = glm::uvec4(0u, 0u, shadowPrivate.mapSize());

        //        // render opaque shadow
        //        shadowFrameBuffer->setForOpaquePass();
        //        renderer->clearRenderData();
        //        for (const auto& [visualDrawable, modelMatrix] : shadowVisualDrawablesCollector.visualDrawables())
        //        {
        //            if (!visualDrawable->isTransparent() || (shadingMode == ShadingMode::OpaqueAndTransparent))
        //                renderer->addRenderData(programsManager->loadOrGetShadowRenderProgram(
        //                    visualDrawable->vertexAttrubiteSet(),
        //                    visualDrawable->uniformCollection(),
        //                    shadingMode,
        //                    shadingFilter),
        //                    visualDrawable,
        //                    modelMatrix);
        //        }
        //        renderer->render(shadowFrameBuffer->frameBuffer(),
        //            shadowViewport,
        //            shadowRenderInfo);

        //        // render transparent shadow
        //        if (shadingMode == ShadingMode::Color)
        //        {
        //            shadowFrameBuffer->setForTransparentPass();
        //            renderer->clearRenderData();
        //            for (const auto& [visualDrawable, modelMatrix] : shadowVisualDrawablesCollector.visualDrawables())
        //            {
        //                if (visualDrawable->isTransparent())
        //                    renderer->addRenderData(programsManager->loadOrGetShadowRenderProgram(
        //                        visualDrawable->vertexAttrubiteSet(),
        //                        visualDrawable->uniformCollection(),
        //                        shadingMode,
        //                        shadingFilter),
        //                        visualDrawable,
        //                        modelMatrix);
        //            }
        //            renderer->render(shadowFrameBuffer->frameBuffer(),
        //                shadowViewport,
        //                shadowRenderInfo);
        //        }

        //        // update light drawable
        //        auto& shadowBuffer = shadowPrivate.shadowBuffer();
        //        
        //        auto shadowDepthMap = shadowBuffer->depthTexture();
        //        auto shadowColorMap = shadowBuffer->colorTexture();

        //        if (shadingFilter == ShadingFilter::VSM)
        //        {
        //            auto& blur = shadowPrivate.blur();
        //            blur->setSourceMap(shadowBuffer->depthVSMTexture(), 0u, true);
        //            shadowDepthMap = blur->run(renderer, m_->programsManager());
        //        }

        //        static const auto s_biasTransform = utils::Transform::makeTranslation(glm::vec3(.5f)) * utils::Transform::makeScale(.5f);
        //        auto& lightDrawable = lightPrivate.areaDrawable();
        //        lightDrawable->setShadowDepthMap(shadowDepthMap);
        //        lightDrawable->setShadowColorMap(shadowColorMap);
        //        lightDrawable->setShadowViewMatrix(shadowTransform.frustumViewTransform);
        //        lightDrawable->setShadowProjectionMatrix(s_biasTransform * shadowProjectionMatrix);
        //        lightDrawable->setShadowDepthBias(shadow.depthBias());
        //    }

        //    auto lightType = lightNode->type();
        //    auto modelMatrix = lightNode->globalTransform() * lightPrivate.areaMatrix();
        //    auto& lightDrawable = lightPrivate.areaDrawable();
        //    auto attributesSet = lightDrawable->vertexAttrubiteSet();

        //    // stencil pass
        //    cameraPrivate.gFrameBuffer()->setForStencilPass();
        //    renderer->clearRenderData();
        //    renderer->addRenderData(programsManager->loadOrGetStencilPassRenderProgram(attributesSet,
        //        lightType),
        //        lightDrawable,
        //        modelMatrix);
        //    renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //        glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //        cameraRenderInfo);

        //    // light pass
        //    cameraPrivate.gFrameBuffer()->setForLightPass();
        //    renderer->clearRenderData();
        //    renderer->addRenderData(programsManager->loadOrGetLightPassRenderProgram(attributesSet,
        //        lightDrawable->uniformCollection(),
        //        lightType,
        //        shadingMode,
        //        shadingFilter,
        //        ssao.mode()),
        //        lightDrawable,
        //        modelMatrix);
        //    renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //        glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //        cameraRenderInfo);

        //    ++cameraInfo.numLightsRendered;
        //}

        //// render final layer
        //cameraPrivate.gFrameBuffer()->setForFinalPass();
        //renderer->clearRenderData();
        //renderer->addRenderData(programsManager->loadOrGetFinalPassRenderProgram(
        //    m_->screenQuadDrawable()->vertexAttrubiteSet()),
        //    m_->screenQuadDrawable());
        //renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(),
        //    glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
        //    cameraRenderInfo);

        //// postprocess
        //cameraPrivate.postprocessFrameBuffer()->setForPass();
        //renderer->clearRenderData();
        //renderer->addRenderData(programsManager->loadOrGetPostprocessPassRenderProgram(
        //    m_->screenQuadDrawable()->vertexAttrubiteSet()),
        //    m_->screenQuadDrawable());
        //renderer->render(cameraPrivate.postprocessFrameBuffer()->frameBuffer(),
        //    glm::uvec4(0u, 0u, screenSize),
        //    cameraRenderInfo);

        
        m_->frameBuffer()->detachAll();
        m_->frameBuffer()->attach(graphics::FrameBufferAttachment::Color0, cameraGeometryBuffer->finalTexture());

        m_->renderer()->blitFrameBuffer(
            m_->frameBuffer(),
            widget->defaultFrameBuffer(),
            glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
            glm::uvec4(0u, 0u, screenSize),
            true, false, false, false);

        sceneInfo.camerasInformation.push_back(cameraInfo);
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
