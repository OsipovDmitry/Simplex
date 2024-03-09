#include <algorithm>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/image.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>
#include <utils/primitiveset.h>
#include <utils/frustum.h>
#include <utils/textfile.h>
#include <utils/shader.h>

#include <core/igraphicsrenderer.h>
#include <core/renderinfo.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/pointlightnode.h>
#include <core/spotlightnode.h>
#include <core/directionallightnode.h>
#include <core/drawablenode.h>
#include <core/standarddrawable.h>
#include <core/backgrounddrawable.h>
#include <core/lightdrawable.h>
#include <core/nodevisitor.h>
#include <core/nodecollector.h>
#include <core/texturesmanager.h>
#include <core/programsmanager.h>
#include <core/uniform.h>

#include "nodevisitorhelpers.h"
#include "graphicsengineprivate.h"
#include "scenerootnodeprivate.h"
#include "cameranodeprivate.h"
#include "backgrounddrawableprivate.h"
#include "pointlightnodeprivate.h"
#include "spotlightnodeprivate.h"
#include "directionallightnodeprivate.h"
#include "ibllightnodeprivate.h"


#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "gltf/gltf.h"

namespace simplex
{
namespace core
{

GraphicsEngine::GraphicsEngine(const std::string &name, std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<GraphicsEnginePrivate>(name))
{
    if (!renderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    m_->renderer() = renderer;

    auto texturesManager = std::make_shared<TexturesManager>(renderer);
    m_->texturesManager() = texturesManager;

    auto programsManager = std::make_shared<ProgramsManager>(renderer);
    m_->programsManager() = programsManager;

    static const uint32_t s_OITMaxNumNodes = 1920 * 1080 * 4;
    static const size_t s_OITDataBufferSize = sizeof(s_OITMaxNumNodes) + s_OITMaxNumNodes * 20u; // 20 bytes per node
    auto OITNodesBuffer = renderer->createBuffer(s_OITDataBufferSize);
    reinterpret_cast<uint32_t*>(OITNodesBuffer->map(graphics::IBuffer::MapAccess::WriteOnly, 0u, sizeof(uint32_t))->get())[0] = s_OITMaxNumNodes;
    m_->OITNodesBuffer() = renderer->createBufferRange(OITNodesBuffer, 0u);
    m_->OITNodesCounter() = renderer->createBufferRange(renderer->createBuffer(sizeof(uint32_t)), 0u);

    static const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> s_screenQuadVertexDeclaration{
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}};
    m_->screenQuadVertexArray() = renderer->createVertexArray(utils::MeshPainter(utils::Mesh::createEmptyMesh(s_screenQuadVertexDeclaration)).drawScreenQuad().mesh());

    BackgroundDrawablePrivate::screenQuadVertexArray() = m_->screenQuadVertexArray();
    BackgroundDrawablePrivate::defaultColorMap() = texturesManager->loadOrGetDefaultIBLEnvironmentTexture();

    static const std::unordered_map<utils::VertexAttribute, std::tuple<uint32_t, utils::VertexComponentType>> s_lightAreaVertexDeclaration{
        {utils::VertexAttribute::Position, {3u, utils::VertexComponentType::Single}}};

    utils::MeshPainter pointLightAreaPainter(utils::Mesh::createEmptyMesh(s_lightAreaVertexDeclaration));
    m_->pointLightAreaVertexArray() = renderer->createVertexArray(pointLightAreaPainter.drawSphere(8u).mesh());
    PointLightNodePrivate::lightAreaVertexArray() = m_->pointLightAreaVertexArray();
    PointLightNodePrivate::lightAreaBoundingBox() = pointLightAreaPainter.calculateBoundingBox();

    utils::MeshPainter spotLightAreaPainter(utils::Mesh::createEmptyMesh(s_lightAreaVertexDeclaration));
    m_->spotLightAreaVertexArray() = renderer->createVertexArray(spotLightAreaPainter.drawCone(8u).mesh());
    SpotLightNodePrivate::lightAreaVertexArray() = m_->spotLightAreaVertexArray();
    SpotLightNodePrivate::lightAreaBoundingBox() = spotLightAreaPainter.calculateBoundingBox();

    utils::MeshPainter directionalLightAreaPainter(utils::Mesh::createEmptyMesh(s_lightAreaVertexDeclaration));
    m_->directionalLightAreaVertexArray() = renderer->createVertexArray(directionalLightAreaPainter.drawCube(glm::vec3(2.f)).mesh());
    DirectionalLightNodePrivate::lightAreaVertexArray() = m_->directionalLightAreaVertexArray();

    IBLLightNodePrivate::lightAreaVertexArray() = DirectionalLightNodePrivate::lightAreaVertexArray(); // the same for IBL light type
    IBLLightNodePrivate::defaultBRDFLutMap() = texturesManager->loadOrGetDefaultIBLBRDFLutTexture();
    IBLLightNodePrivate::defaultDiffuseMap() = texturesManager->loadOrGetDefaultIBLDiffuseTexture();
    IBLLightNodePrivate::defaultSpecularMap() = texturesManager->loadOrGetDefaultIBLSpecularTexture();

    m_->screenQuadDrawable() = std::make_shared<Drawable>(m_->screenQuadVertexArray());

    m_->finalScreenQuadDrawable() = std::make_shared<Drawable>(m_->screenQuadVertexArray());

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
    static utils::Transform vm;
    static glm::mat4x4 pm;
    static bool isR = false;

    auto &debugInfo = m_->debugInformation();
    debugInfo.graphicsEngineName = name();
    debugInfo.scenesInformation.clear();

    auto renderer = m_->renderer();
    auto texturesManager = m_->texturesManager();
    auto programsManager = m_->programsManager();

    const auto &screenSize = renderer->viewportSize();

    auto sceneList = scenes();
    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObjectComparator());

    for (auto &scene : sceneList)
    {
        debug::SceneInformation sceneInfo;
        sceneInfo.sceneName = scene->name();

        auto rootNode = scene->sceneRootNode();
        auto backgroundDrawable = scene->backgroundDrawable();

        // update nodes
        UpdateNodeVisitor nodeUpdateVisitor(time, dt);
        rootNode->acceptDown(nodeUpdateVisitor);

        // render
        NodeCollector<CameraNode> cameraNodeCollector;
        rootNode->acceptDown(cameraNodeCollector);
        std::stable_sort(cameraNodeCollector.nodes().begin(), cameraNodeCollector.nodes().end(), utils::SortedObjectComparator());

        for (const auto &camera : cameraNodeCollector.nodes())
        {
            auto &cameraPrivate = camera->m();

            debug::CameraInformation cameraInfo;
            cameraInfo.cameraName = camera->name();
            auto &numOpaqueDrawablesRendered = cameraInfo.numOpaqueDrawablesRendered;
            numOpaqueDrawablesRendered = 0u;
            auto &numTransparentDrawablesRendered = cameraInfo.numTransparentDrawablesRendererd;
            numTransparentDrawablesRendered = 0u;
            auto &numLightsRendered = cameraInfo.numLightsRendered;
            numLightsRendered = 0u;

            if (!camera->isRenderingEnabled())
                continue;

            camera->resize(screenSize);

            const auto cameraViewportSize = camera->viewportSize();
            const auto cameraViewport = glm::uvec4(0u, 0u, cameraViewportSize);
            float aspectRatio = static_cast<float>(cameraViewportSize[0]) / static_cast<float>(cameraViewportSize[1]);
            /*const*/ auto cameraViewTransform = camera->globalTransform().inverted();

            ZRangeNodeVisitor zRangeNodeVisitor(utils::OpenFrustum(camera->calculateProjectionMatrix(aspectRatio, utils::Range(0.f, 1.f)) *
                                                                   cameraViewTransform));
            rootNode->acceptDown(zRangeNodeVisitor);

            utils::Range cameraZRange(.5f, 1.f);
            if (!zRangeNodeVisitor.zRange().isEmpty())
            {
                const auto &cameraCullPlanesLimits = camera->cullPlanesLimits();

                cameraZRange.setNearValue(glm::max(cameraCullPlanesLimits.nearValue(), zRangeNodeVisitor.zRange().nearValue() * 0.95f));
                cameraZRange.setFarValue(glm::min(cameraCullPlanesLimits.farValue(), zRangeNodeVisitor.zRange().farValue() * 1.05f));
            }

            /*const*/ auto cameraProjectionMatrix = camera->calculateProjectionMatrix(aspectRatio, cameraZRange);


            if (isR)
            {
                cameraViewTransform = vm;
                cameraProjectionMatrix = pm;
            }

            RenderInfo renderInfo;
            renderInfo.setViewMatrix(cameraViewTransform);
            renderInfo.setProjectionMatrix(cameraProjectionMatrix);

            renderInfo.setGBuffer(cameraPrivate.gFrameBuffer()->color0Texture(),
                                  cameraPrivate.gFrameBuffer()->color1Texture(),
                                  cameraPrivate.gFrameBuffer()->color2Texture(),
                                  cameraPrivate.gFrameBuffer()->depthTexture());

            renderInfo.setOITBuffer(cameraPrivate.oitFrameBuffer()->oitDepthImage(),
                                    cameraPrivate.oitFrameBuffer()->oitIndicesImage(),
                                    m_->OITNodesBuffer(),
                                    m_->OITNodesCounter());

            renderInfo.setLightBufferColorTexture(cameraPrivate.lightFrameBuffer()->colorTexture());
            renderInfo.setFinalBufferColorTexture(cameraPrivate.finalFrameBuffer()->colorTexture());

            const auto cameraFrustum = utils::Frustum(renderInfo.viewProjectionMatrixUniform()->data());
            const auto cameraFrustumCornersInfo = utils::Frustum::calculateCornersInfo(renderInfo.viewProjectionMatrixInverseUniform()->data());

            DrawableNodesCollector drawableNodesCollector(cameraFrustum);
            rootNode->acceptDown(drawableNodesCollector);

            LightNodesCollector lightNodesCollector(cameraFrustum);
            rootNode->acceptDown(lightNodesCollector);

            // render opaque geometry to G-buffer
            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodesCollector.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                {
                    const auto alphaMode = drawable->alphaMode();
                    if ((alphaMode == DrawableAlphaMode::Opaque) || (alphaMode == DrawableAlphaMode::Mask))
                    {
                        renderer->addRenderData(programsManager->loadOrGetOpaqueGeometryPassRenderProgram(
                                                    Drawable::vertexAttrubitesSet(drawable),
                                                    StandardDrawable::PBRComponentsSet(drawable)),
                                                drawable,
                                                drawableNode->globalTransform());
                        ++numOpaqueDrawablesRendered;
                    }
                }
            renderer->render(cameraPrivate.gFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            // reset OIT counter buffer
            auto nodecCounterBuffer = m_->OITNodesCounter()->buffer();
            if (auto nodesCounterBufferData = nodecCounterBuffer->map(graphics::IBuffer::MapAccess::WriteOnly); nodesCounterBufferData)
                std::memset(nodesCounterBufferData->get(), 0, nodecCounterBuffer->size());

            // clear OIT images
            renderer->compute(programsManager->loadOrGetOITClearPassComputeProgram(),
                              renderInfo,
                              glm::uvec3(cameraViewportSize, 1u));

            // render transparent geometry to OIT-buffer
            renderer->clearRenderData();
            for (const auto &drawableNode : drawableNodesCollector.drawableNodes())
                for (const auto &drawable : drawableNode->drawables())
                    if (drawable->alphaMode() == DrawableAlphaMode::Transparent)
                    {
                        renderer->addRenderData(programsManager->loadOrGetTransparentGeometryPassRenderProgram(
                                                    Drawable::vertexAttrubitesSet(drawable),
                                                    StandardDrawable::PBRComponentsSet(drawable)),
                                                drawable,
                                                drawableNode->globalTransform());
                        ++numTransparentDrawablesRendered;
                    }
            renderer->render(cameraPrivate.oitFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            // get number of transparent pixels rendered
            if (auto nodesCounterBufferData = nodecCounterBuffer->map(graphics::IBuffer::MapAccess::ReadOnly); nodesCounterBufferData)
                cameraInfo.numTrasparentPixelsRendered = *reinterpret_cast<uint32_t*>(nodesCounterBufferData->get());

            // sort OIT nodes
            renderer->compute(programsManager->loadOrGetOITSortNodesPassComputeProgram(),
                              renderInfo,
                              glm::uvec3(cameraViewportSize, 1u));

            // clear light color buffer
            cameraPrivate.lightFrameBuffer()->setForClearPass();
            renderer->clearRenderData();
            renderer->render(cameraPrivate.lightFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            // render lights areas
            for (const auto &lightNode : lightNodesCollector.lightNodes())
            {
                if (!lightNode->isLightingEnabled())
                    continue;

                if (lightNode->isShadingEnabled())
                {
                    const auto shadowViewTransform = lightNode->calculateShadowViewTransform(cameraFrustumCornersInfo);
                    ZRangeNodeVisitor shadowZRangeNodeVisitor(
                                utils::OpenFrustum(lightNode->calculateShadowProjectionMatrix(shadowViewTransform,
                                                                                              cameraFrustumCornersInfo,
                                                                                              utils::Range(0.f, 1.f)) *
                                                   shadowViewTransform));
                    rootNode->acceptDown(shadowZRangeNodeVisitor);

                    utils::Range shadowZRange(.5f, 1.f);
                    if (!shadowZRangeNodeVisitor.zRange().isEmpty())
                    {
                        shadowZRange.setNearValue(shadowZRangeNodeVisitor.zRange().nearValue() * 0.95f);
                        shadowZRange.setFarValue(shadowZRangeNodeVisitor.zRange().farValue() * 1.05f);
                    }

                    const auto shadowProjectionMatrix = lightNode->calculateShadowProjectionMatrix(shadowViewTransform,
                                                                                                   cameraFrustumCornersInfo,
                                                                                                   shadowZRange);

                    if (debug)
                    {
                        vm = shadowViewTransform;
                        pm = shadowProjectionMatrix;
                        debug = false;
                        isR = true;
                        return;
                    }

                }

                auto modelMatrix = lightNode->globalTransform() * lightNode->areaMatrix();
                const auto &drawable = lightNode->areaDrawable();
                auto attributesSet = Drawable::vertexAttrubitesSet(drawable);

                // stencil pass
                cameraPrivate.lightFrameBuffer()->setForStencilPass();
                renderer->clearRenderData();
                renderer->addRenderData(programsManager->loadOrGetStencilPassRenderProgram(attributesSet),
                                        drawable,
                                        modelMatrix);
                renderer->render(cameraPrivate.lightFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

                // light pass
                cameraPrivate.lightFrameBuffer()->setForLightPass();
                renderer->clearRenderData();
                renderer->addRenderData(programsManager->loadOrGetLightPassRenderProgram(attributesSet,
                                                                                         LightDrawable::lightComponentsSet(drawable),
                                                                                         drawable->type()),
                                        drawable,
                                        modelMatrix);
                renderer->render(cameraPrivate.lightFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);
                ++numLightsRendered;
            }

            // render background layer
            cameraPrivate.finalFrameBuffer()->setForBackgroundPass();
            renderer->clearRenderData();
            renderer->addRenderData(programsManager->loadOrGetBackgroundPassRenderProgram(
                                        Drawable::vertexAttrubitesSet(backgroundDrawable),
                                        BackgroundDrawable::backgroundComponentsSet(backgroundDrawable)),
                                    backgroundDrawable);
            renderer->render(cameraPrivate.finalFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            // render foreground layer
            cameraPrivate.finalFrameBuffer()->setForForegroundPass();
            renderer->clearRenderData();
            renderer->addRenderData(programsManager->loadOrGetForegroundPassRenderProgram(
                                        Drawable::vertexAttrubitesSet(m_->screenQuadDrawable())),
                                    m_->screenQuadDrawable());
            renderer->render(cameraPrivate.finalFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            // postprocess
            renderer->clearRenderData();
            renderer->addRenderData(programsManager->loadOrGetPostprocessPassRenderProgram(
                                        Drawable::vertexAttrubitesSet(m_->screenQuadDrawable())),
                                    m_->screenQuadDrawable());
            renderer->render(cameraPrivate.postprocessFrameBuffer()->frameBuffer(), renderInfo, cameraViewport);

            sceneInfo.camerasInformation.push_back(cameraInfo);
        }

        debugInfo.scenesInformation.push_back(sceneInfo);
    }

    renderer->clearRenderData();
}

const std::shared_ptr<graphics::IRenderer> &GraphicsEngine::graphicsRenderer() const
{
    return m_->renderer();
}

const std::shared_ptr<TexturesManager> &GraphicsEngine::texturesManager() const
{
    return m_->texturesManager();
}

const std::shared_ptr<ProgramsManager> &GraphicsEngine::programsManager() const
{
    return m_->programsManager();
}

const std::vector<std::shared_ptr<Scene>> &GraphicsEngine::scenes() const
{
    return m_->scenes();
}

std::shared_ptr<Scene> GraphicsEngine::addEmptyScene(const std::string &name)
{
    auto scene = std::shared_ptr<Scene>(new Scene(shared_from_this(), name));
    scene->sceneRootNode()->m().scene() = scene;
    m_->scenes().push_back(scene);

    return scene;
}

std::shared_ptr<Scene> GraphicsEngine::loadGLTFSceneFromFile(const std::filesystem::path &filename)
{
    static const auto insertNode = [](const std::shared_ptr<graphics::IRenderer> &renderer,
                                      const std::vector<std::shared_ptr<graphics::IBuffer>> &buffers,
                                      const std::vector<graphics::PTexture> &textures,
                                      const std::shared_ptr<Node> &parentNode,
                                      const tinygltf::Model &rawModel,
                                      int rawNodeIndex) {
        static const auto insertNodeImpl = [](const auto &impl,
                                              const std::shared_ptr<graphics::IRenderer> &renderer,
                                              const std::vector<std::shared_ptr<graphics::IBuffer>> &buffers,
                                              const std::vector<graphics::PTexture> &textures,
                                              const std::shared_ptr<Node> &parentNode,
                                              const tinygltf::Model &rawModel,
                                              int rawNodeIndex) -> void
        {
            static const auto tinyType2NumComponents = [](int value) -> uint32_t
            {
                static const std::unordered_map<int, uint32_t> s_table {
                    {TINYGLTF_TYPE_SCALAR, 1u},
                    {TINYGLTF_TYPE_VEC2, 2u},
                    {TINYGLTF_TYPE_VEC3, 3u},
                    {TINYGLTF_TYPE_VEC4, 4u},
                };

                const auto it = s_table.find(value);
                return it != s_table.end() ? it->second : 0u;
            };

            static const auto tinyComponentType2VertexComponentType = [](int value) -> utils::VertexComponentType
            {
                static const std::unordered_map<int, utils::VertexComponentType> s_table {
                    {TINYGLTF_COMPONENT_TYPE_INT, utils::VertexComponentType::Int32},
                    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, utils::VertexComponentType::Uint32},
                    {TINYGLTF_COMPONENT_TYPE_FLOAT, utils::VertexComponentType::Single},
                    {TINYGLTF_COMPONENT_TYPE_DOUBLE, utils::VertexComponentType::Double},
                };

                const auto it = s_table.find(value);
                return it != s_table.end() ? it->second : utils::VertexComponentType::Count;
            };

            static const auto tinyComponentType2DrawElementsIndexType = [](int value) -> utils::DrawElementsIndexType
            {
                static const std::unordered_map<int, utils::DrawElementsIndexType> s_table {
                    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, utils::DrawElementsIndexType::Uint8},
                    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, utils::DrawElementsIndexType::Uint16},
                    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, utils::DrawElementsIndexType::Uint32},
                };

                const auto it = s_table.find(value);
                return it != s_table.end() ? it->second : utils::DrawElementsIndexType::Undefined;
            };

            static const auto tinyAttribute2VertexAttribute = [](const std::string &value) -> utils::VertexAttribute
            {
                static const std::unordered_map<std::string, utils::VertexAttribute> s_table {
                    {"POSITION", utils::VertexAttribute::Position},
                    {"NORMAL", utils::VertexAttribute::Normal},
                    {"TANGENT", utils::VertexAttribute::Tangent},
                    {"TEXCOORD_0", utils::VertexAttribute::TexCoords},
                    {"COLOR_0", utils::VertexAttribute::Color},
                    {"JOINTS_0", utils::VertexAttribute::BonesIDs},
                    {"WEIGHTS_0", utils::VertexAttribute::BonesWeights},
                };

                const auto it = s_table.find(value);
                return it != s_table.end() ? it->second : utils::VertexAttribute::Count;
            };

            static const auto tinyMode2PrimitiveType = [](int value) -> utils::PrimitiveType
            {
                static const std::unordered_map<int, utils::PrimitiveType> s_table {
                    {TINYGLTF_MODE_POINTS, utils::PrimitiveType::Points},
                    {TINYGLTF_MODE_LINE, utils::PrimitiveType::Lines},
                    {TINYGLTF_MODE_LINE_STRIP, utils::PrimitiveType::LineStrip},
                    {TINYGLTF_MODE_TRIANGLES, utils::PrimitiveType::Triangles},
                    {TINYGLTF_MODE_TRIANGLE_STRIP, utils::PrimitiveType::TriangleStrip},
                    {TINYGLTF_MODE_TRIANGLE_FAN, utils::PrimitiveType::TriangleFan},
                };

                const auto it = s_table.find(value);
                return it != s_table.end() ? it->second : utils::PrimitiveType::Undefined;
            };

            const auto &rawNode = rawModel.nodes[static_cast<size_t>(rawNodeIndex)];

            std::shared_ptr<Node> node;
            if (rawNode.camera >= 0)
            {
                const auto &rawCamera = rawModel.cameras[static_cast<size_t>(rawNode.camera)];
                auto cameraNode = std::make_shared<CameraNode>(rawNode.name);

                if (rawCamera.type == "perspective")
                    cameraNode->setPerspectiveProjection(static_cast<float>(rawCamera.perspective.yfov));
                else if (rawCamera.type == "orthographic")
                    cameraNode->setOrthoProjection();
                else
                    LOG_CRITICAL << "Undefined type of camera";

                node = cameraNode;
            }
            else if (rawNode.mesh >= 0)
            {
                const auto &rawMesh = rawModel.meshes[static_cast<size_t>(rawNode.mesh)];

                auto drawableNode = std::make_shared<DrawableNode>(rawNode.name);
                for (const auto &rawPrimitive : rawMesh.primitives)
                {
                    utils::BoundingBox boundingBox;
                    uint32_t numVertices = 0u;
                    auto vao = renderer->createVertexArray();

                    for (const auto& [rawAttributeName, accessorIndex] : rawPrimitive.attributes)
                    {
                        const auto &rawAccessor = rawModel.accessors[static_cast<size_t>(accessorIndex)];
                        const auto &rawBufferView = rawModel.bufferViews[static_cast<size_t>(rawAccessor.bufferView)];

                        const auto bindingPoint = vao->attachVertexBuffer(buffers[static_cast<size_t>(rawBufferView.buffer)],
                                                                          rawBufferView.byteOffset,
                                                                          static_cast<uint32_t>(rawAccessor.ByteStride(rawBufferView)));

                        auto vertexAttribute = tinyAttribute2VertexAttribute(rawAttributeName);
                        if (vertexAttribute == utils::VertexAttribute::Count)
                            LOG_CRITICAL << "Undefined type of vertex attribute";
                        if (vertexAttribute == utils::VertexAttribute::Position)
                        {
                            numVertices = static_cast<uint32_t>(rawAccessor.count);
                            boundingBox = utils::BoundingBox(glm::vec3(static_cast<float>(rawAccessor.minValues[0u]),
                                                                       static_cast<float>(rawAccessor.minValues[1u]),
                                                                       static_cast<float>(rawAccessor.minValues[2u])),
                                                             glm::vec3(static_cast<float>(rawAccessor.maxValues[0u]),
                                                                       static_cast<float>(rawAccessor.maxValues[1u]),
                                                                       static_cast<float>(rawAccessor.maxValues[2u])));
                        }

                        const auto numComponents = tinyType2NumComponents(rawAccessor.type);
                        if (!numComponents)
                            LOG_CRITICAL << "Undefined number of components of vertex attribute";

                        const auto vertexComponentType = tinyComponentType2VertexComponentType(rawAccessor.componentType);
                        if (vertexComponentType == utils::VertexComponentType::Count)
                            LOG_CRITICAL << "Undefined component type of vertex attribute";

                        vao->declareVertexAttribute(vertexAttribute,
                                                    bindingPoint,
                                                    numComponents,
                                                    vertexComponentType,
                                                    0u);
                    }

                    auto primitiveType = tinyMode2PrimitiveType(rawPrimitive.mode);
                    if (primitiveType == utils::PrimitiveType::Undefined)
                        LOG_CRITICAL << "Undefined primitive type";

                    if (rawPrimitive.indices >= 0)
                    {
                        const auto &rawAccessor = rawModel.accessors[static_cast<size_t>(rawPrimitive.indices)];
                        const auto &rawBufferView = rawModel.bufferViews[static_cast<size_t>(rawAccessor.bufferView)];
                        vao->attachIndexBuffer(buffers[static_cast<size_t>(rawBufferView.buffer)]);

                        const auto drawElementsIndexType = tinyComponentType2DrawElementsIndexType(rawAccessor.componentType);
                        if (drawElementsIndexType == utils::DrawElementsIndexType::Undefined)
                            LOG_CRITICAL << "Undefined component type of draw elements index buffer";

                        vao->addPrimitiveSet(std::make_shared<utils::DrawElements>(primitiveType,
                                                                                   static_cast<uint32_t>(rawAccessor.count),
                                                                                   drawElementsIndexType,
                                                                                   rawBufferView.byteOffset + rawAccessor.byteOffset,
                                                                                   0u));
                    }
                    else
                    {
                        vao->addPrimitiveSet(std::make_shared<utils::DrawArrays>(primitiveType, 0u, numVertices));
                    }

                    auto standardDrawable = std::make_shared<StandardDrawable>(vao, boundingBox);
                    standardDrawable->setORMSwizzleMask(glm::uvec4(0u, 1u, 2u, 3u));
                    drawableNode->addDrawable(standardDrawable);

                    if (rawPrimitive.material >= 0)
                    {
                        const auto &rawMaterial = rawModel.materials[static_cast<size_t>(rawPrimitive.material)];

                        if (rawMaterial.alphaMode == "MASK")
                            standardDrawable->setAlphaCutoff(static_cast<float>(rawMaterial.alphaCutoff));

                        const auto &rawNormalTextureInfo = rawMaterial.normalTexture;
                        if (rawNormalTextureInfo.index >= 0)
                        {
                            if (rawNormalTextureInfo.texCoord != 0)
                                LOG_CRITICAL << "Multiple texture coords are not supported";

                            standardDrawable->setNormalMap(textures[static_cast<size_t>(rawNormalTextureInfo.index)]);
                            standardDrawable->setNormalMapScale(static_cast<float>(rawNormalTextureInfo.scale));
                        }

                        const auto &rawOcclusionTextureInfo = rawMaterial.occlusionTexture;
                        if (rawOcclusionTextureInfo.index >= 0)
                        {
                            if (rawOcclusionTextureInfo.texCoord != 0)
                                LOG_CRITICAL << "Multiple texture coords are not supported";

                            standardDrawable->setOcclusionMap(textures[static_cast<size_t>(rawOcclusionTextureInfo.index)]);
                            standardDrawable->setOcclusionMapStrength(static_cast<float>(rawOcclusionTextureInfo.strength));
                        }

                        const auto &rawPBR = rawMaterial.pbrMetallicRoughness;

                        standardDrawable->setBaseColor(glm::vec4(static_cast<float>(rawPBR.baseColorFactor[0u]),
                                                                 static_cast<float>(rawPBR.baseColorFactor[1u]),
                                                                 static_cast<float>(rawPBR.baseColorFactor[2u]),
                                                                 static_cast<float>(rawPBR.baseColorFactor[3u])));
                        standardDrawable->setMetalness(static_cast<float>(rawPBR.metallicFactor));
                        standardDrawable->setRoughness(static_cast<float>(rawPBR.roughnessFactor));

                        const auto &rawBaseColorTextureInfo = rawPBR.baseColorTexture;
                        if (rawBaseColorTextureInfo.index >= 0)
                        {
                            if (rawBaseColorTextureInfo.texCoord != 0)
                                LOG_CRITICAL << "Multiple texture coords are not supported";

                            standardDrawable->setBaseColorMap(textures[static_cast<size_t>(rawBaseColorTextureInfo.index)]);
                        }

                        const auto &rawMetalnessRoughnessTextureInfo = rawPBR.metallicRoughnessTexture;
                        if (rawMetalnessRoughnessTextureInfo.index >= 0)
                        {
                            if (rawMetalnessRoughnessTextureInfo.texCoord != 0)
                                LOG_CRITICAL << "Multiple texture coords are not supported";

                            const auto &metalnessRoughnessTexture = textures[static_cast<size_t>(rawMetalnessRoughnessTextureInfo.index)];
                            standardDrawable->setMetalnessMap(metalnessRoughnessTexture);
                            standardDrawable->setRoughnessMap(metalnessRoughnessTexture);
                        }
                    }
                }

                node = drawableNode;
            }
            else if (rawNode.light >= 0)
            {
                const auto &rawLight = rawModel.lights[static_cast<size_t>(rawNode.light)];
                std::shared_ptr<LightNode> lightNode;

                if (rawLight.type == "point")
                {
                    auto pointLightNode = std::make_shared<PointLightNode>(rawNode.name);
                    pointLightNode->setRadiuses(glm::vec2(0.f, static_cast<float>(rawLight.range)));
                    lightNode = pointLightNode;
                }
                else if (rawLight.type == "spot")
                {
                    auto spotLightNode = std::make_shared<SpotLightNode>(rawNode.name);
                    spotLightNode->setRadiuses(glm::vec2(0.f, static_cast<float>(rawLight.range)));
                    spotLightNode->setHalfAngles(glm::vec2(static_cast<float>(rawLight.spot.innerConeAngle),
                                                           static_cast<float>(rawLight.spot.outerConeAngle)));
                    lightNode = spotLightNode;
                }
                else if (rawLight.type == "directional")
                {
                    auto directionalLightNode = std::make_shared<DirectionalLightNode>(rawNode.name);
                    lightNode = directionalLightNode;
                }
                else
                    LOG_CRITICAL << "Undefined type of light";

                glm::vec3 lightColor(static_cast<float>(rawLight.intensity));
                if (!rawLight.color.empty())
                    lightColor *= glm::vec3(static_cast<float>(rawLight.color[0u]),
                                            static_cast<float>(rawLight.color[1u]),
                                            static_cast<float>(rawLight.color[2u]));

                node = lightNode;
            }
            else if (rawNode.skin >= 0)
            {
                node = std::make_shared<Node>(rawNode.name);
            }
            else if (rawNode.emitter >= 0)
            {
                node = std::make_shared<Node>(rawNode.name);
            }
            else
            {
                node = std::make_shared<Node>(rawNode.name);
            }

            utils::Transform nodeTransform;
            if (!rawNode.scale.empty())
                nodeTransform *= utils::Transform::fromScale(static_cast<float>(rawNode.scale[0u]));
            if (!rawNode.rotation.empty())
                nodeTransform *= utils::Transform::fromRotation(glm::quat(static_cast<float>(rawNode.rotation[3u]),
                                                                          static_cast<float>(rawNode.rotation[0u]),
                                                                          static_cast<float>(rawNode.rotation[1u]),
                                                                          static_cast<float>(rawNode.rotation[2u])));
            if (!rawNode.translation.empty())
                nodeTransform *= utils::Transform::fromTranslation(glm::vec3(static_cast<float>(rawNode.translation[0u]),
                                                                             static_cast<float>(rawNode.translation[1u]),
                                                                             static_cast<float>(rawNode.translation[2u])));
            node->setTransform(nodeTransform);

            parentNode->attach(node);
            for (auto &rawChildNodeIndex : rawNode.children)
                impl(impl, renderer, buffers, textures, node, rawModel, rawChildNodeIndex);

        };

        insertNodeImpl(insertNodeImpl, renderer, buffers, textures, parentNode, rawModel, rawNodeIndex);
    };

    const auto absoluteFilename = std::filesystem::absolute(filename);
    const auto absoluteParentDir = absoluteFilename.parent_path();

    const auto &renderer = graphicsRenderer();

    tinygltf::Model rawModel;

    auto gltfFile = utils::TextFile::loadFromFile(absoluteFilename);
    if (gltfFile)
    {
        std::string errorString, warningString;

        const auto &data = gltfFile->data();
        auto loadingResult = tinygltf::TinyGLTF().LoadASCIIFromString(&rawModel,
                                                                      &errorString,
                                                                      &warningString,
                                                                      data.c_str(),
                                                                      static_cast<unsigned int>(data.size()),
                                                                      absoluteParentDir.string());

        if (!warningString.empty())
            LOG_WARNING << warningString;

        if (!errorString.empty())
            LOG_ERROR << errorString;

        if (!loadingResult)
            LOG_CRITICAL << "Failed to load glTF scene " << filename;
    }
    else
        LOG_CRITICAL << "Can't open glTF scene file " << absoluteFilename;

    if (rawModel.scenes.empty())
        LOG_CRITICAL << "glTF scene file " << absoluteFilename << "doesn't have any scenes";

    const uint32_t defaultSceneIndex = rawModel.defaultScene >= 0 ? static_cast<uint32_t>(rawModel.defaultScene) : 0u;
    const auto &rawScene = rawModel.scenes[defaultSceneIndex];

    std::vector<std::shared_ptr<graphics::IBuffer>> buffers(rawModel.buffers.size());
    for (size_t i = 0u; i < rawModel.buffers.size(); ++i)
    {
        const auto &rawBuffer = rawModel.buffers[i];
        buffers[i] = renderer->createBuffer(rawBuffer.data.size(), rawBuffer.data.data());
    }

    std::vector<graphics::PTexture> textures(rawModel.textures.size());
    for (size_t i = 0u; i < rawModel.textures.size(); ++i)
    {
        const auto &rawTexture = rawModel.textures[i];
        const auto &rawImage = rawModel.images[static_cast<size_t>(rawTexture.source)];
        textures[i] = texturesManager()->loadOrGetTexture(absoluteParentDir / rawImage.uri);
    }

    auto scene = addEmptyScene(rawScene.name);
    auto sceneRootNode = scene->sceneRootNode();

    for (auto rawChildNodeIndex : rawScene.nodes)
        insertNode(renderer, buffers, textures, sceneRootNode, rawModel, rawChildNodeIndex);

    return scene;
}

void GraphicsEngine::removeScene(std::shared_ptr<Scene> scene)
{
    if (auto it = std::find(m_->scenes().begin(), m_->scenes().end(), scene); it != m_->scenes().end())
        m_->scenes().erase(it);
    else
        LOG_ERROR << "Graphics engine \"" << name() << "\" doesn't have scene \"" << scene->name() << "\"";
}

const debug::GraphicsEngineInformation &GraphicsEngine::debugInformation() const
{
    return m_->debugInformation();
}

void GraphicsEngine::setF(int i)
{
    m_->finalScreenQuadDrawable()->getOrCreateUserUniform("i") = makeUniform(i);
}

}
}
