#include <queue>

#include <utils/logger.h>
#include <utils/clipspace.h>
#include <utils/textfile.h>

#include <core/drawable.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/listenernode.h>
#include <core/settings.h>
#include <core/graphicsrendererbase.h>
#include <core/texturesmanager.h>
#include <core/cameranode.h>
#include <core/pbrdrawable.h>
#include <core/visualdrawablenode.h>
#include <core/pointlightnode.h>
#include <core/directionallightnode.h>
#include <core/spotlightnode.h>
#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/background.h>

#include "sceneprivate.h"
#include "scenerootnodeprivate.h"
#include "scenedata.h"

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

Scene::~Scene() = default;

const std::string &Scene::name() const
{
    return m_->name();
}

std::shared_ptr<SceneRootNode> Scene::sceneRootNode()
{
    return m_->sceneRootNode();
}

std::shared_ptr<const SceneRootNode> Scene::sceneRootNode() const
{
    return const_cast<Scene*>(this)->sceneRootNode();
}

std::shared_ptr<ListenerNode> Scene::listenerNode()
{
    return m_->listenerNode();
}

std::shared_ptr<const ListenerNode> Scene::listenerNode() const
{
    return const_cast<Scene*>(this)->listenerNode();
}

std::shared_ptr<Background> Scene::background()
{
    return m_->background();
}

std::shared_ptr<const Background> Scene::background() const
{
    return const_cast<Scene*>(this)->background();
}

std::shared_ptr<Scene> Scene::createEmpty(const std::string& sceneName)
{
    auto scene = std::shared_ptr<Scene>(new Scene(sceneName));
    scene->m().sceneRootNode()->m().scene() = scene;

    return scene;
}

std::shared_ptr<Scene> Scene::createFromGLTF(const std::filesystem::path& filename,
    const std::shared_ptr<graphics::IFrameBuffer>& defaultFrameBuffer)
{
    static const auto tinyType2NumComponents = [](int value) -> uint32_t
        {
            static const std::unordered_map<int, uint32_t> s_table{
                {TINYGLTF_TYPE_SCALAR, 1u},
                {TINYGLTF_TYPE_VEC2, 2u},
                {TINYGLTF_TYPE_VEC3, 3u},
                {TINYGLTF_TYPE_VEC4, 4u},
            };

            const auto it = s_table.find(value);
            return it != s_table.end() ? it->second : 0u;
        };

    static const auto createBuffer = [](const std::shared_ptr<graphics::RendererBase>& renderer,
        const tinygltf::Model& rawModel,
        size_t rawBufferIndex) -> std::shared_ptr<graphics::IBuffer>
        {
            const auto& rawBuffer = rawModel.buffers[rawBufferIndex];
            return renderer->createStaticBuffer(rawBuffer.data.size(), rawBuffer.data.data());
        };

    static const auto createTexture = [](const std::shared_ptr<TexturesManager>& texturesManager,
        const std::filesystem::path& absoluteParentDir,
        const tinygltf::Model& rawModel,
        size_t rawTextureIndex) -> graphics::PTexture
        {
            const auto& rawTexture = rawModel.textures[rawTextureIndex];
            const auto& rawImage = rawModel.images[static_cast<size_t>(rawTexture.source)];
            return texturesManager->loadOrGetTexture(absoluteParentDir / rawImage.uri);
        };

    static const auto createNode = [](const std::shared_ptr<graphics::RendererBase>& renderer,
        const std::shared_ptr<graphics::IFrameBuffer>& defaultFrameBuffer,
        const std::vector<std::shared_ptr<graphics::IBuffer>>& buffers,
        const std::vector<graphics::PTexture>& textures,
        const tinygltf::Model& rawModel,
        size_t rawNodeIndex) -> std::shared_ptr<Node>
        {
            static const auto tinyComponentType2VertexComponentType = [](int value) -> utils::VertexComponentType
                {
                    static const std::unordered_map<int, utils::VertexComponentType> s_table{
                        {TINYGLTF_COMPONENT_TYPE_BYTE, utils::VertexComponentType::Int8},
                        {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, utils::VertexComponentType::Uint8},
                        {TINYGLTF_COMPONENT_TYPE_SHORT, utils::VertexComponentType::Int16},
                        {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, utils::VertexComponentType::Uint16},
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
                    static const std::unordered_map<int, utils::DrawElementsIndexType> s_table{
                        {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, utils::DrawElementsIndexType::Uint8},
                        {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, utils::DrawElementsIndexType::Uint16},
                        {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, utils::DrawElementsIndexType::Uint32},
                    };

                    const auto it = s_table.find(value);
                    return it != s_table.end() ? it->second : utils::DrawElementsIndexType::Count;
                };

            static const auto tinyAttribute2VertexAttribute = [](const std::string& value) -> utils::VertexAttribute
                {
                    static const std::unordered_map<std::string, utils::VertexAttribute> s_table{
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
                    static const std::unordered_map<int, utils::PrimitiveType> s_table{
                        {TINYGLTF_MODE_POINTS, utils::PrimitiveType::Points},
                        {TINYGLTF_MODE_LINE, utils::PrimitiveType::Lines},
                        {TINYGLTF_MODE_LINE_STRIP, utils::PrimitiveType::LineStrip},
                        {TINYGLTF_MODE_TRIANGLES, utils::PrimitiveType::Triangles},
                        {TINYGLTF_MODE_TRIANGLE_STRIP, utils::PrimitiveType::TriangleStrip},
                        {TINYGLTF_MODE_TRIANGLE_FAN, utils::PrimitiveType::TriangleFan},
                    };

                    const auto it = s_table.find(value);
                    return it != s_table.end() ? it->second : utils::PrimitiveType::Count;
                };

            const auto& rawNode = rawModel.nodes[rawNodeIndex];
            std::shared_ptr<Node> node;

            if (rawNode.camera >= 0)
            {
                const auto& rawCamera = rawModel.cameras[static_cast<size_t>(rawNode.camera)];
                auto cameraNode = std::make_shared<CameraNode>(rawNode.name, defaultFrameBuffer);

                if (rawCamera.type == "perspective")
                    cameraNode->setPerspectiveClipSpace(static_cast<utils::ClipSpace::value_type>(rawCamera.perspective.yfov));
                else if (rawCamera.type == "orthographic")
                    cameraNode->setOrthoClipSpace(static_cast<utils::ClipSpace::value_type>(rawCamera.orthographic.ymag));
                else
                {
                    LOG_ERROR << "Undefined type of camera";
                    return nullptr;
                }

                node = cameraNode;
            }
            else if (rawNode.mesh >= 0)
            {
                const auto& rawMesh = rawModel.meshes[static_cast<size_t>(rawNode.mesh)];

                auto visualDrawableNode = std::make_shared<VisualDrawableNode>(rawNode.name);
                for (const auto& rawPrimitive : rawMesh.primitives)
                {
                    utils::BoundingBox boundingBox;
                    uint32_t numVertices = 0u;

                    auto VAOMesh = graphics::VAOMesh::create();
                    auto vao = VAOMesh->vao();

                    for (const auto& [rawAttributeName, accessorIndex] : rawPrimitive.attributes)
                    {
                        const auto& rawAccessor = rawModel.accessors[static_cast<size_t>(accessorIndex)];
                        const auto& rawBufferView = rawModel.bufferViews[static_cast<size_t>(rawAccessor.bufferView)];

                        const auto bindingPoint = vao->attachVertexBuffer(buffers[static_cast<size_t>(rawBufferView.buffer)],
                            rawBufferView.byteOffset,
                            static_cast<uint32_t>(rawAccessor.ByteStride(rawBufferView)));

                        auto vertexAttribute = tinyAttribute2VertexAttribute(rawAttributeName);
                        if (vertexAttribute == utils::VertexAttribute::Count)
                        {
                            LOG_ERROR << "Undefined type of vertex attribute";
                            return nullptr;
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
                            return nullptr;
                        }

                        const auto vertexComponentType = tinyComponentType2VertexComponentType(rawAccessor.componentType);
                        if (vertexComponentType == utils::VertexComponentType::Count)
                        {
                            LOG_ERROR << "Undefined component type of vertex attribute";
                            return nullptr;
                        }

                        vao->declareVertexAttribute(vertexAttribute,
                            bindingPoint,
                            numComponents,
                            vertexComponentType,
                            0u);
                    }

                    auto primitiveType = tinyMode2PrimitiveType(rawPrimitive.mode);
                    if (primitiveType == utils::PrimitiveType::Count)
                    {
                        LOG_ERROR << "Undefined primitive type";
                        return nullptr;
                    }

                    if (rawPrimitive.indices >= 0)
                    {
                        const auto& rawAccessor = rawModel.accessors[static_cast<size_t>(rawPrimitive.indices)];
                        const auto& rawBufferView = rawModel.bufferViews[static_cast<size_t>(rawAccessor.bufferView)];
                        vao->attachIndexBuffer(buffers[static_cast<size_t>(rawBufferView.buffer)]);

                        const auto drawElementsIndexType = tinyComponentType2DrawElementsIndexType(rawAccessor.componentType);
                        if (drawElementsIndexType == utils::DrawElementsIndexType::Count)
                        {
                            LOG_ERROR << "Undefined component type of draw elements index buffer";
                            return nullptr;
                        }

                        VAOMesh->addPrimitiveSet(std::make_shared<utils::DrawElements>(primitiveType,
                            static_cast<uint32_t>(rawAccessor.count),
                            drawElementsIndexType,
                            rawBufferView.byteOffset + rawAccessor.byteOffset,
                            0u));
                    }
                    else
                    {
                        VAOMesh->addPrimitiveSet(std::make_shared<utils::DrawArrays>(primitiveType, 0u, numVertices));
                    }

                    auto pbrDrawable = std::make_shared<PBRDrawable>(VAOMesh, boundingBox);
                    pbrDrawable->setORMSwizzleMask(glm::uvec4(0u, 1u, 2u, 3u));
                    visualDrawableNode->addVisualDrawable(pbrDrawable);

                    if (rawPrimitive.material >= 0)
                    {
                        const auto& rawMaterial = rawModel.materials[static_cast<size_t>(rawPrimitive.material)];

                        if (rawMaterial.alphaMode == "MASK")
                            pbrDrawable->setAlphaCutoff(static_cast<float>(rawMaterial.alphaCutoff));

                        pbrDrawable->setDoubleSided(rawMaterial.doubleSided);

                        const auto& rawEmissionTextureInfo = rawMaterial.emissiveTexture;
                        if (rawEmissionTextureInfo.index >= 0)
                        {
                            if (rawEmissionTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return nullptr;
                            }

                            pbrDrawable->setEmissionMap(textures[static_cast<size_t>(rawEmissionTextureInfo.index)]);
                            pbrDrawable->setEmission(glm::vec3(static_cast<float>(rawMaterial.emissiveFactor[0u]),
                                static_cast<float>(rawMaterial.emissiveFactor[1u]),
                                static_cast<float>(rawMaterial.emissiveFactor[2u])));
                        }

                        const auto& rawNormalTextureInfo = rawMaterial.normalTexture;
                        if (rawNormalTextureInfo.index >= 0)
                        {
                            if (rawNormalTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return nullptr;
                            }

                            pbrDrawable->setNormalMap(textures[static_cast<size_t>(rawNormalTextureInfo.index)]);
                            pbrDrawable->setNormalMapScale(static_cast<float>(rawNormalTextureInfo.scale));
                        }

                        const auto& rawOcclusionTextureInfo = rawMaterial.occlusionTexture;
                        if (rawOcclusionTextureInfo.index >= 0)
                        {
                            if (rawOcclusionTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return nullptr;
                            }

                            pbrDrawable->setOcclusionMap(textures[static_cast<size_t>(rawOcclusionTextureInfo.index)]);
                            pbrDrawable->setOcclusionMapStrength(static_cast<float>(rawOcclusionTextureInfo.strength));
                        }

                        const auto& rawPBR = rawMaterial.pbrMetallicRoughness;

                        pbrDrawable->setBaseColor(glm::vec4(static_cast<float>(rawPBR.baseColorFactor[0u]),
                            static_cast<float>(rawPBR.baseColorFactor[1u]),
                            static_cast<float>(rawPBR.baseColorFactor[2u]),
                            static_cast<float>(rawPBR.baseColorFactor[3u])));
                        pbrDrawable->setMetalness(static_cast<float>(rawPBR.metallicFactor));
                        pbrDrawable->setRoughness(static_cast<float>(rawPBR.roughnessFactor));

                        const auto& rawBaseColorTextureInfo = rawPBR.baseColorTexture;
                        if (rawBaseColorTextureInfo.index >= 0)
                        {
                            if (rawBaseColorTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return nullptr;
                            }

                            pbrDrawable->setBaseColorMap(textures[static_cast<size_t>(rawBaseColorTextureInfo.index)]);
                        }

                        const auto& rawMetalnessRoughnessTextureInfo = rawPBR.metallicRoughnessTexture;
                        if (rawMetalnessRoughnessTextureInfo.index >= 0)
                        {
                            if (rawMetalnessRoughnessTextureInfo.texCoord != 0)
                            {
                                LOG_ERROR << "Multiple texture coords are not supported";
                                return nullptr;
                            }

                            const auto& metalnessRoughnessTexture = textures[static_cast<size_t>(rawMetalnessRoughnessTextureInfo.index)];
                            pbrDrawable->setMetalnessMap(metalnessRoughnessTexture);
                            pbrDrawable->setRoughnessMap(metalnessRoughnessTexture);
                        }
                    }
                }

                node = visualDrawableNode;
            }
            else if (rawNode.light >= 0)
            {
                const auto& rawLight = rawModel.lights[static_cast<size_t>(rawNode.light)];

                glm::vec3 lightColor(static_cast<float>(rawLight.intensity));
                if (!rawLight.color.empty())
                    lightColor *= glm::vec3(static_cast<float>(rawLight.color[0u]),
                        static_cast<float>(rawLight.color[1u]),
                        static_cast<float>(rawLight.color[2u]));

                std::shared_ptr<LightNode> lightNode;

                if (rawLight.type == "point")
                {
                    auto pointLightNode = std::make_shared<PointLightNode>(rawNode.name);
                    pointLightNode->setRadiuses(glm::vec2(0.f, static_cast<float>(rawLight.range)));
                    pointLightNode->setColor(lightColor);
                    lightNode = pointLightNode;
                }
                else if (rawLight.type == "spot")
                {
                    auto spotLightNode = std::make_shared<SpotLightNode>(rawNode.name);
                    spotLightNode->setRadiuses(glm::vec2(0.f, static_cast<float>(rawLight.range)));
                    spotLightNode->setHalfAngles(glm::vec2(static_cast<float>(rawLight.spot.innerConeAngle),
                        static_cast<float>(rawLight.spot.outerConeAngle)));
                    spotLightNode->setColor(lightColor);
                    lightNode = spotLightNode;
                }
                else if (rawLight.type == "directional")
                {
                    auto directionalLightNode = std::make_shared<DirectionalLightNode>(rawNode.name);
                    directionalLightNode->setColor(lightColor);
                    lightNode = directionalLightNode;
                }
                else
                {
                    LOG_ERROR << "Undefined type of light";
                    return nullptr;
                }

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

            return node;
        };

    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    auto graphicsWidget = graphicsRenderer->widget();
    if (!graphicsWidget)
        LOG_CRITICAL << "Graphics widget can't be nullptr";

    auto graphicsEngine = graphicsWidget->graphicsEngine();
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    const auto absoluteFilename = std::filesystem::absolute(filename);
    const auto absoluteParentDir = absoluteFilename.parent_path();

    auto renderer = graphicsEngine->graphicsRenderer();
    auto texturesManager = graphicsEngine->texturesManager();

    tinygltf::Model rawModel;

    auto gltfFile = utils::TextFile::loadFromFile(absoluteFilename);
    if (gltfFile)
    {
        std::string errorString, warningString;

        const auto& data = gltfFile->data();
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

    std::vector<std::shared_ptr<graphics::IBuffer>> buffers(rawModel.buffers.size());
    for (size_t i = 0u; i < rawModel.buffers.size(); ++i)
    {
        auto buffer = createBuffer(renderer, rawModel, i);
        if (!buffer)
            return nullptr;
        buffers[i] = buffer;
    }

    std::vector<graphics::PTexture> textures(rawModel.textures.size());
    for (size_t i = 0u; i < rawModel.textures.size(); ++i)
    {
        auto texture = createTexture(texturesManager, absoluteParentDir, rawModel, i);
        if (!texture)
            return nullptr;
        textures[i] = texture;
    }

    std::vector<std::shared_ptr<Node>> nodes(rawModel.nodes.size());
    for (size_t i = 0u; i < rawModel.nodes.size(); ++i)
    {
        auto node = createNode(renderer, defaultFrameBuffer, buffers, textures, rawModel, i);
        if (!node)
            return nullptr;
        nodes[i] = node;
    }

    const uint32_t defaultSceneIndex = rawModel.defaultScene >= 0 ? static_cast<uint32_t>(rawModel.defaultScene) : 0u;
    const auto& rawScene = rawModel.scenes[defaultSceneIndex];

    auto scene = createEmpty(rawScene.name);
    auto sceneRootNode = scene->sceneRootNode();

    std::queue<int> nodesQueue;
    for (auto nodeIndex : rawScene.nodes)
    {
        sceneRootNode->attach(nodes[static_cast<size_t>(nodeIndex)]);
        nodesQueue.push(nodeIndex);
    }

    while (!nodesQueue.empty())
    {
        auto parentNodeIndex = static_cast<size_t>(nodesQueue.front());
        nodesQueue.pop();

        auto parentNode = nodes[parentNodeIndex];

        for (auto nodeIndex : rawModel.nodes[parentNodeIndex].children)
        {
            parentNode->attach(nodes[static_cast<size_t>(nodeIndex)]);
            nodesQueue.push(nodeIndex);
        }
    }

    return scene;
}


Scene::Scene(const std::string &name)
    : m_(std::make_unique<ScenePrivate>(*this, name))
{
    m_->background() = std::make_shared<Background>();
    m_->sceneData()->setBackground(m_->background());

    m_->listenerNode() = std::shared_ptr<ListenerNode>(new ListenerNode(settings::Settings::instance().application().scene().listenerNodeName()));

    m_->sceneRootNode() = std::shared_ptr<SceneRootNode>(new SceneRootNode(settings::Settings::instance().application().scene().sceneRootNodeName()));
    m_->sceneRootNode()->attach(m_->listenerNode());
}



}
}
