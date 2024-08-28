#include <utils/logger.h>
#include <utils/textfile.h>
#include <utils/mesh.h>
#include <utils/boundingbox.h>
#include <utils/clipspace.h>

#include <core/applicationbase.h>
#include <core/settings.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/igraphicsrenderer.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/cameranode.h>
#include <core/visualdrawablenode.h>
#include <core/pointlightnode.h>
#include <core/spotlightnode.h>
#include <core/directionallightnode.h>
#include <core/pbrdrawable.h>

#include "applicationbaseprivate.h"
#include "sceneprivate.h"
#include "scenerootnodeprivate.h"
#include "nodevisitorhelpers.h"

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

ApplicationBase::ApplicationBase(const std::string &name)
    : m_(std::make_unique<ApplicationBasePrivate>(name))
{
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been created";
}

ApplicationBase::~ApplicationBase()
{
    ApplicationBase::shutDown();
    LOG_INFO << "Application \"" << ApplicationBase::name() << "\" has been destroyed";
}

const std::string &ApplicationBase::name() const
{
    return m_->name();
}

std::shared_ptr<ApplicationBase> ApplicationBase::currentApplication()
{
    auto &currentApplicationWeak = ApplicationBasePrivate::currentApplication();
    return currentApplicationWeak.expired() ? nullptr : currentApplicationWeak.lock();
}

void ApplicationBase::shutDown()
{
    m_->scenes().clear();

    while (!m_->engines().empty())
        unregisterEngine(*m_->engines().begin());
}

void ApplicationBase::update(const std::shared_ptr<IRenderWidget> &renderWidget, uint64_t time, uint32_t dt)
{
    ApplicationBasePrivate::currentApplication() = weak_from_this();

    if (auto &isInitialized = m_->isInitialized(); !isInitialized)
    {
        doInitialize();
        isInitialized = true;
    }

    auto &debugInfo = m_->debugInformation();
    debugInfo.scenesInformation.clear();

    std::vector<std::shared_ptr<Scene>> sceneList { m_->scenes().begin(), m_->scenes().end() };
    std::stable_sort(sceneList.begin(), sceneList.end(), utils::SortedObjectComparator());

    for (auto &scene : sceneList)
    {
        debug::SceneInformation sceneInfo;
        sceneInfo.sceneName = scene->name();
        debugInfo.scenesInformation.push_back(sceneInfo);

        // update nodes
        UpdateNodeVisitor nodeUpdateVisitor(time, dt);
        scene->sceneRootNode()->acceptDown(nodeUpdateVisitor);

        for (const auto &engine : m_->engines())
            engine->update(renderWidget, scene, time, dt, sceneInfo);

        doUpdate(time, dt);
    }

    ApplicationBasePrivate::currentApplication().reset();
}

const std::unordered_set<std::shared_ptr<IEngine>> &ApplicationBase::engines() const
{
    return m_->engines();
}

bool ApplicationBase::registerEngine(std::shared_ptr<IEngine> value)
{
    if (auto it = m_->engines().find(value); it != m_->engines().end())
    {
        LOG_ERROR << "Engine \"" << (*it)->name() << "\" is already registered";
        return false;
    }

    m_->engines().insert(value);
    LOG_INFO << "Engine \"" << value->name() << "\" has been registered to application \"" << ApplicationBase::name() << "\"";

    return true;
}

bool ApplicationBase::unregisterEngine(std::shared_ptr<IEngine> value)
{
    if (auto it = m_->engines().find(value); it == m_->engines().end())
    {
        LOG_ERROR << "Engine \"" << (*it)->name() << "\" is not registered";
        return false;
    }

    m_->engines().erase(value);
    LOG_INFO << "Engine \"" << value->name() << "\" has been unregistered from application \"" << ApplicationBase::name() << "\"";

    return true;
}

std::shared_ptr<IEngine> ApplicationBase::findEngine(const std::string &name)
{
    return findEngine<IEngine>(name);
}

const std::unordered_set<std::shared_ptr<Scene>> &ApplicationBase::scenes() const
{
    return m_->scenes();
}

void ApplicationBase::removeScene(const std::shared_ptr<Scene> &value)
{
    if (auto it = m_->scenes().find(value); it != m_->scenes().end())
        m_->scenes().erase(it);
    else
        LOG_ERROR << "Application \"" << name() << "\" doesn't have scene \"" << value->name() << "\"";
}

std::shared_ptr<Scene> ApplicationBase::loadEmptyScene(const std::string &sceneName)
{
    auto scene = std::shared_ptr<Scene>(new Scene(weak_from_this(), sceneName));
    scene->m().sceneRootNode()->m().scene() = scene;

    m_->scenes().insert(scene);
    return scene;
}

std::shared_ptr<Scene> ApplicationBase::loadGLTFScene(const std::filesystem::path &filename,
                                                      const std::shared_ptr<graphics::IFrameBuffer> &defaultFrameBuffer)
{
    static const auto insertNode = [](const std::shared_ptr<graphics::IRenderer> &renderer,
                                      const std::shared_ptr<graphics::IFrameBuffer> &defaultFrameBuffer,
                                      const std::vector<std::shared_ptr<graphics::IBuffer>> &buffers,
                                      const std::vector<graphics::PTexture> &textures,
                                      const std::shared_ptr<Node> &parentNode,
                                      const tinygltf::Model &rawModel,
                                      int rawNodeIndex) -> bool
    {
        static const auto insertNodeImpl = [](const auto &impl,
                                              const std::shared_ptr<graphics::IRenderer> &renderer,
                                              const std::shared_ptr<graphics::IFrameBuffer> &defaultFrameBuffer,
                                              const std::vector<std::shared_ptr<graphics::IBuffer>> &buffers,
                                              const std::vector<graphics::PTexture> &textures,
                                              const std::shared_ptr<Node> &parentNode,
                                              const tinygltf::Model &rawModel,
                                              int rawNodeIndex) -> bool
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
                auto cameraNode = std::make_shared<CameraNode>(rawNode.name, defaultFrameBuffer);

                if (rawCamera.type == "perspective")
                    cameraNode->setPerspectiveClipSpace(static_cast<utils::ClipSpace::value_type>(rawCamera.perspective.yfov));
                else if (rawCamera.type == "orthographic")
                    cameraNode->setOrthoClipSpace(static_cast<utils::ClipSpace::value_type>(rawCamera.orthographic.ymag));
                else
                {
                    LOG_ERROR << "Undefined type of camera";
                    return false;
                }

                node = cameraNode;
            }
            else if (rawNode.mesh >= 0)
            {
                const auto &rawMesh = rawModel.meshes[static_cast<size_t>(rawNode.mesh)];

                auto visualDrawableNode = std::make_shared<VisualDrawableNode>(rawNode.name);
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
                        {
                            LOG_ERROR << "Undefined type of vertex attribute";
                            return false;
                        }
                        if (vertexAttribute == utils::VertexAttribute::Position)
                        {
                            numVertices = static_cast<uint32_t>(rawAccessor.count);
                            boundingBox = utils::BoundingBox({ glm::vec3(static_cast<float>(rawAccessor.minValues[0u]),
                                                                         static_cast<float>(rawAccessor.minValues[1u]),
                                                                         static_cast<float>(rawAccessor.minValues[2u])),
                                                               glm::vec3(static_cast<float>(rawAccessor.maxValues[0u]),
                                                                         static_cast<float>(rawAccessor.maxValues[1u]),
                                                                         static_cast<float>(rawAccessor.maxValues[2u])) });
                        }

                        const auto numComponents = tinyType2NumComponents(rawAccessor.type);
                        if (!numComponents)
                        {
                            LOG_ERROR << "Undefined number of components of vertex attribute";
                            return false;
                        }

                        const auto vertexComponentType = tinyComponentType2VertexComponentType(rawAccessor.componentType);
                        if (vertexComponentType == utils::VertexComponentType::Count)
                        {
                            LOG_ERROR << "Undefined component type of vertex attribute";
                            return false;
                        }

                        vao->declareVertexAttribute(vertexAttribute,
                                                    bindingPoint,
                                                    numComponents,
                                                    vertexComponentType,
                                                    0u);
                    }

                    auto primitiveType = tinyMode2PrimitiveType(rawPrimitive.mode);
                    if (primitiveType == utils::PrimitiveType::Undefined)
                    {
                        LOG_ERROR << "Undefined primitive type";
                        return false;
                    }

                    if (rawPrimitive.indices >= 0)
                    {
                        const auto &rawAccessor = rawModel.accessors[static_cast<size_t>(rawPrimitive.indices)];
                        const auto &rawBufferView = rawModel.bufferViews[static_cast<size_t>(rawAccessor.bufferView)];
                        vao->attachIndexBuffer(buffers[static_cast<size_t>(rawBufferView.buffer)]);

                        const auto drawElementsIndexType = tinyComponentType2DrawElementsIndexType(rawAccessor.componentType);
                        if (drawElementsIndexType == utils::DrawElementsIndexType::Undefined)
                        {
                            LOG_ERROR << "Undefined component type of draw elements index buffer";
                            return false;
                        }

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

                    auto pbrDrawable = std::make_shared<PBRDrawable>(vao, boundingBox);
                    pbrDrawable->setORMSwizzleMask(glm::uvec4(0u, 1u, 2u, 3u));
                    visualDrawableNode->addVisualDrawable(pbrDrawable);

                    if (rawPrimitive.material >= 0)
                    {
                        const auto &rawMaterial = rawModel.materials[static_cast<size_t>(rawPrimitive.material)];

                        if (rawMaterial.alphaMode == "MASK")
                            pbrDrawable->setAlphaCutoff(static_cast<float>(rawMaterial.alphaCutoff));

                        const auto &rawEmissionTextureInfo = rawMaterial.emissiveTexture;
                        if (rawEmissionTextureInfo.index >= 0)
                        {
                            if (rawEmissionTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return false;
                            }

                            pbrDrawable->setEmissionMap(textures[static_cast<size_t>(rawEmissionTextureInfo.index)]);
                            pbrDrawable->setEmission(glm::vec3(static_cast<float>(rawMaterial.emissiveFactor[0u]),
                                                               static_cast<float>(rawMaterial.emissiveFactor[1u]),
                                                               static_cast<float>(rawMaterial.emissiveFactor[2u])));
                        }

                        const auto &rawNormalTextureInfo = rawMaterial.normalTexture;
                        if (rawNormalTextureInfo.index >= 0)
                        {
                            if (rawNormalTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return false;
                            }

                            pbrDrawable->setNormalMap(textures[static_cast<size_t>(rawNormalTextureInfo.index)]);
                            pbrDrawable->setNormalMapScale(static_cast<float>(rawNormalTextureInfo.scale));
                        }

                        const auto &rawOcclusionTextureInfo = rawMaterial.occlusionTexture;
                        if (rawOcclusionTextureInfo.index >= 0)
                        {
                            if (rawOcclusionTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return false;
                            }

                            pbrDrawable->setOcclusionMap(textures[static_cast<size_t>(rawOcclusionTextureInfo.index)]);
                            pbrDrawable->setOcclusionMapStrength(static_cast<float>(rawOcclusionTextureInfo.strength));
                        }

                        const auto &rawPBR = rawMaterial.pbrMetallicRoughness;

                        pbrDrawable->setBaseColor(glm::vec4(static_cast<float>(rawPBR.baseColorFactor[0u]),
                                                            static_cast<float>(rawPBR.baseColorFactor[1u]),
                                                            static_cast<float>(rawPBR.baseColorFactor[2u]),
                                                            static_cast<float>(rawPBR.baseColorFactor[3u])));
                        pbrDrawable->setMetalness(static_cast<float>(rawPBR.metallicFactor));
                        pbrDrawable->setRoughness(static_cast<float>(rawPBR.roughnessFactor));

                        const auto &rawBaseColorTextureInfo = rawPBR.baseColorTexture;
                        if (rawBaseColorTextureInfo.index >= 0)
                        {
                            if (rawBaseColorTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return false;
                            }

                            pbrDrawable->setBaseColorMap(textures[static_cast<size_t>(rawBaseColorTextureInfo.index)]);
                        }

                        const auto &rawMetalnessRoughnessTextureInfo = rawPBR.metallicRoughnessTexture;
                        if (rawMetalnessRoughnessTextureInfo.index >= 0)
                        {
                            if (rawMetalnessRoughnessTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return false;
                            }

                            const auto &metalnessRoughnessTexture = textures[static_cast<size_t>(rawMetalnessRoughnessTextureInfo.index)];
                            pbrDrawable->setMetalnessMap(metalnessRoughnessTexture);
                            pbrDrawable->setRoughnessMap(metalnessRoughnessTexture);
                        }
                    }
                }

                node = visualDrawableNode;
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
                {
                    LOG_ERROR << "Undefined type of light";
                    return false;
                }

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
                nodeTransform *= utils::Transform::makeScale(static_cast<float>(rawNode.scale[0u]));
            if (!rawNode.rotation.empty())
                nodeTransform *= utils::Transform::makeRotation(glm::quat(static_cast<float>(rawNode.rotation[3u]),
                                                                          static_cast<float>(rawNode.rotation[0u]),
                                                                          static_cast<float>(rawNode.rotation[1u]),
                                                                          static_cast<float>(rawNode.rotation[2u])));
            if (!rawNode.translation.empty())
                nodeTransform *= utils::Transform::makeTranslation(glm::vec3(static_cast<float>(rawNode.translation[0u]),
                                                                             static_cast<float>(rawNode.translation[1u]),
                                                                             static_cast<float>(rawNode.translation[2u])));
            node->setTransform(nodeTransform);

            parentNode->attach(node);
            for (auto &rawChildNodeIndex : rawNode.children)
                if (!impl(impl, renderer, defaultFrameBuffer, buffers, textures, node, rawModel, rawChildNodeIndex))
                    return false;

            return true;
        };

        return insertNodeImpl(insertNodeImpl, renderer, defaultFrameBuffer, buffers, textures, parentNode, rawModel, rawNodeIndex);
    };

    auto graphicsEngine = findEngine<GraphicsEngine>();
    if (!graphicsEngine)
    {
        LOG_ERROR << "Failed to create GLTF scene " << filename << ". Application " << name() << " doesn't have Graphics engine";
        return nullptr;
    }

    const auto absoluteFilename = std::filesystem::absolute(filename);
    const auto absoluteParentDir = absoluteFilename.parent_path();

    auto renderer = graphicsEngine->graphicsRenderer();
    auto texturesManager = graphicsEngine->texturesManager();

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
        {
            LOG_ERROR << "Failed to load glTF scene " << filename << ": " << errorString;
            return nullptr;
        }

        if (!loadingResult)
        {
            LOG_ERROR << "Failed to load glTF scene " << filename;
            return nullptr;
        }
    }
    else
    {
        LOG_ERROR << "Can't open glTF scene file " << absoluteFilename;
        return nullptr;
    }

    if (rawModel.scenes.empty())
    {
        LOG_ERROR << "glTF scene file " << absoluteFilename << "doesn't have any scenes";
        return nullptr;
    }

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
        textures[i] = texturesManager->loadOrGetTexture(absoluteParentDir / rawImage.uri);
    }

    auto scene = loadEmptyScene(rawScene.name);
    auto sceneRootNode = scene->sceneRootNode();

    for (auto rawChildNodeIndex : rawScene.nodes)
        if (!insertNode(renderer, defaultFrameBuffer, buffers, textures, sceneRootNode, rawModel, rawChildNodeIndex))
            return nullptr;

    return scene;
}

void ApplicationBase::doInitialize()
{
}

void ApplicationBase::doUpdate(uint64_t /*time*/, uint32_t /*dt*/)
{
}

}
}
