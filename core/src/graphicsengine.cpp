#include <algorithm>

#include <utils/logger.h>
#include <utils/meshpainter.h>
#include <utils/orientedboundingbox.h>

#include <core/cameranode.h>
#include <core/debuginformation.h>
#include <core/drawablenode.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/igraphicswidget.h>
#include <core/lightnode.h>
#include <core/nodecollector.h>
#include <core/programsloader.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/scenesloader.h>
#include <core/settings.h>
#include <core/texturesloader.h>
#include <core/uniform.h>

#include "cameranodeprivate.h"
#include "geometrybuffer.h"
#include "graphicsengineprivate.h"
#include "lightnodeprivate.h"
#include "nodevisitorhelpers.h"
#include "renderpipeline.h"
#include "scenedata.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string& name, const std::shared_ptr<graphics::RendererBase>& renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    static const auto& settings = settings::Settings::instance();
    static const auto& debugRenderSettings = settings.graphics().debugRendering();

    if (!renderer) LOG_CRITICAL << "Graphics renderer can't be nullptr";

    m_->renderer() = renderer;
    m_->renderer()->makeCurrent();

    for (typename std::underlying_type<utils::VertexAttribute>::type i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (const auto& name = GraphicsEnginePrivate::attributeNameByID(utils::castToVertexAttribute(i)); !name.empty())
            renderer->registerAttribute(name, utils::castToVertexAttribute(i));
        else
            LOG_ERROR << "Attribute name has not been set for ID = " << i;

    for (typename std::underlying_type<graphics::FrameBufferAttachment>::type i = 0u;
         i < graphics::FrameBufferColorAttachmentsCount(); ++i)
        if (const auto& name = GraphicsEnginePrivate::outputNameByID(graphics::FrameBufferColorAttachment(i)); !name.empty())
            renderer->registerOutput(name, graphics::castToFrameBufferAttachment(i));
        else
            LOG_ERROR << "Output name has not been set for ID = " << i;

    for (typename std::underlying_type<UniformID>::type i = 0u; i < numElementsUniformID(); ++i)
        if (const auto& name = GraphicsEnginePrivate::uniformNameByID(castToUniformID(i)); !name.empty())
            renderer->registerUniform(name, core::castToUniformID(i));
        else
            LOG_ERROR << "Uniform name has not been set for ID = " << i;

    for (typename std::underlying_type<UniformBlockID>::type i = 0u; i < numElementsUniformBlockID(); ++i)
        if (const auto& name = GraphicsEnginePrivate::uniformBlockNameByID(castToUniformBlockID(i)); !name.empty())
            renderer->registerUniformBlock(name, core::castToUniformBlockID(i));
        else
            LOG_ERROR << "Uniform block name has not been set for ID = " << i;

    for (typename std::underlying_type<ShaderStorageBlockID>::type i = 0u; i < numElementsShaderStorageBlockID(); ++i)
        if (const auto& name = GraphicsEnginePrivate::shaderStorageBlockNameByID(castToShaderStorageBlockID(i)); !name.empty())
            renderer->registerShaderStorageBlock(name, core::castToShaderStorageBlockID(i));
        else
            LOG_ERROR << "Shader storage block name has not been set for ID = " << i;

    m_->programsLoader() = std::make_shared<ProgramsLoader>(name + "ProgramsLoader", renderer);
    m_->texturesLoader() = std::make_shared<TexturesLoader>(name + "TexturesLoader", renderer);
    m_->scenesLoader() = std::make_shared<ScenesLoader>(name + "ScenesLoader");

    m_->frameBuffer() = renderer->createFrameBuffer();
    m_->vertexArray() = renderer->createVertexArray();
    m_->geometryBuffer() = std::make_shared<GeometryBuffer>(glm::uvec2(0u));

    m_->renderPipeLine() = RenderPipeLine::create(m_->programsLoader(), renderer, m_->frameBuffer(), m_->vertexArray());

    m_->dielectricSpecular() = settings.graphics().pbr().dielectricSpecular();

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

std::shared_ptr<TexturesLoader> GraphicsEngine::texturesLoader()
{
    return m_->texturesLoader();
}

std::shared_ptr<const TexturesLoader> GraphicsEngine::texturesLoader() const
{
    return const_cast<GraphicsEngine*>(this)->texturesLoader();
}

std::shared_ptr<ProgramsLoader> GraphicsEngine::programsLoader()
{
    return m_->programsLoader();
}

std::shared_ptr<const ProgramsLoader> GraphicsEngine::programsLoader() const
{
    return const_cast<GraphicsEngine*>(this)->programsLoader();
}

std::shared_ptr<ScenesLoader> GraphicsEngine::scenesLoader()
{
    return m_->scenesLoader();
}

std::shared_ptr<const ScenesLoader> GraphicsEngine::scenesLoader() const
{
    return const_cast<GraphicsEngine*>(this)->scenesLoader();
}

void GraphicsEngine::update(const std::shared_ptr<Scene>& scene, uint64_t time, uint32_t /*dt*/)
{
    if (!scene) return;

    auto renderer = graphicsRenderer();
    if (!renderer)
    {
        LOG_CRITICAL << "Graphics renderer can't be nullptr";
        return;
    }

    auto programsLoader = GraphicsEngine::programsLoader();
    if (!programsLoader)
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

    auto& geometryBuffer = m_->geometryBuffer();
    geometryBuffer->initialize(programsLoader);
    geometryBuffer->resize(screenSize, renderer);

    auto rootNode = scene->sceneRootNode();
    const auto globalBoundingBox = rootNode->globalTransform() * utils::OrientedBoundingBox(rootNode->boundingBox());

    NodeCollector<CameraNode> cameraNodeCollector;
    rootNode->acceptDown(cameraNodeCollector);
    std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObjectComparator());

    for (const auto& camera : cameraNodeCollector.nodes())
    {
        auto& cameraPrivate = camera->m();

        if (!camera->isRenderingEnabled()) continue;

        std::shared_ptr<GeometryBuffer> cameraGeometryBuffer = geometryBuffer;
        if (!camera->isDefaultFramebufferUsed()) cameraGeometryBuffer = cameraPrivate.geometryBuffer();

        cameraPrivate.resize(cameraGeometryBuffer->size());

        auto& renderPipeLine = m_->renderPipeLine();
        renderPipeLine->run(
            cameraGeometryBuffer, scene->m().sceneData(), time, m_->dielectricSpecular(), globalBoundingBox,
            camera->globalTransform().inverted(), camera->clipSpace(), camera->cullPlanesLimits(), camera->clusterSize());

        m_->frameBuffer()->detachAll();
        m_->frameBuffer()->attach(graphics::FrameBufferAttachment::Color0, renderPipeLine->finalTexture());

        m_->renderer()->blitFrameBuffer(
            m_->frameBuffer(), widget->defaultFrameBuffer(), glm::uvec4(0u, 0u, cameraGeometryBuffer->size()),
            glm::uvec4(0u, 0u, screenSize), true, false, false, false);
    }
}

} // namespace core
} // namespace simplex
