#include <unordered_map>

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/boundingbox.h>
#include <utils/mesh.h>
#include <utils/logger.h>

#include <core/animation.h>
#include <core/drawable.h>
#include <core/material.h>
#include <core/mesh.h>
#include <core/scenerepresentation.h>

#include <loader_assimp/loader.h>

namespace simplex
{

namespace loader_assimp
{

struct aiStringHash
{
    std::size_t operator()(const aiString& s) const noexcept
    {
        return std::hash<std::string_view>{}(std::string_view(s.C_Str()));
    }
};

using AssimpFaceIndex = std::remove_pointer<decltype(aiFace::mIndices)>::type;
static const auto s_drawElementsIndexType = utils::toDrawElementsIndexType<AssimpFaceIndex>();
static const auto s_triangleSize = 3u * utils::DrawElements::indexSize(s_drawElementsIndexType);

static std::string toString(const aiString& value)
{
    return value.C_Str();
}

static glm::vec3 toVec3(const aiVector3t<ai_real>& value)
{
    return glm::vec3(value.x, value.y, value.z);
}

static glm::vec3 toVec3(const aiColor3D& value)
{
    return glm::vec3(value.r, value.g, value.b);
}

static glm::vec4 toVec4(const aiColor4D& value)
{
    return glm::vec4(value.r, value.g, value.b, value.a);
}

static glm::quat toQuat(const aiQuaterniont<ai_real>& value)
{
    return glm::quat(value.w, value.x, value.y, value.z);
}

static utils::Transform toTransform(const aiMatrix4x4t<ai_real>& value)
{
    aiVector3t<ai_real> scale;
    aiQuaterniont<ai_real> rotation;
    aiVector3t<ai_real> translation;
    value.Decompose(scale, rotation, translation);

    //tmp
    scale.x = 1.0f;
    //

    return utils::Transform(static_cast<utils::Transform::value_type>(scale.x), toQuat(rotation), toVec3(translation));
}

static utils::BoundingBox toBoundingBox(const aiAABB& value)
{
    return utils::BoundingBox::fromMinMax(toVec3(value.mMin), toVec3(value.mMax));
}

template<uint32_t Dim, typename TReal>
static std::shared_ptr<utils::VertexBuffer> toVertexBuffer(size_t verticesCount, const void* data)
{
    auto vertexBuffer = std::make_shared<utils::VertexBuffer>(verticesCount, Dim, utils::toVertexComponentType<TReal>());
    memcpy(vertexBuffer->data(), data, vertexBuffer->sizeInBytes());
    return vertexBuffer;
}

template<typename TReal>
static std::shared_ptr<utils::VertexBuffer> toVertexBuffer(size_t verticesCount, const aiVector3t<TReal>* data)
{
    return toVertexBuffer<3u, TReal>(verticesCount, reinterpret_cast<const void*>(data));
}

static std::shared_ptr<utils::VertexBuffer> toVertexBuffer(size_t verticesCount, const aiColor4t<float>* data)
{
    return toVertexBuffer<4u, float>(verticesCount, reinterpret_cast<const void*>(data));
}

static std::shared_ptr<utils::DrawElementsBuffer> toDrawElementsBuffer(size_t facesCount, const aiFace* data)
{
    auto drawElementsBuffer = std::make_shared<utils::DrawElementsBuffer>(
        utils::PrimitiveType::Triangles,
        3u * facesCount,
        s_drawElementsIndexType,
        0u);

    for (size_t i = 0u; i < facesCount; ++i)
        memcpy(drawElementsBuffer->data() + s_triangleSize * i, data[i].mIndices, s_triangleSize);

    return drawElementsBuffer;
}

static std::shared_ptr<utils::DrawArrays> toDrawArrays(size_t verticesCount)
{
    return std::make_shared<utils::DrawArrays>(utils::PrimitiveType::Triangles, 0u, verticesCount);
}

static std::shared_ptr<core::Drawable> toDrawable(
    const aiMesh* assimpMesh,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBones,
    const std::vector<std::shared_ptr<core::Material>>& materials)
{
    auto mesh = std::make_shared<utils::Mesh>();

    if (assimpMesh->HasPositions())
        mesh->attachVertexBuffer(utils::VertexAttribute::Position, toVertexBuffer(assimpMesh->mNumVertices, assimpMesh->mVertices));

    if (assimpMesh->HasNormals())
    {
        mesh->attachVertexBuffer(utils::VertexAttribute::Normal, toVertexBuffer(assimpMesh->mNumVertices, assimpMesh->mNormals));

        if (assimpMesh->HasTangentsAndBitangents())
        {
            auto vertexBuffer = std::make_shared<utils::VertexBuffer>(assimpMesh->mNumVertices, 4u, utils::toVertexComponentType<ai_real>());
            for (size_t i = 0u; i < assimpMesh->mNumVertices; ++i)
            {
                std::array<ai_real, 4u> tangent {
                    assimpMesh->mTangents[i][0u],
                    assimpMesh->mTangents[i][1u],
                    assimpMesh->mTangents[i][2u],
                    static_cast<ai_real>(1.0) };
                if ((assimpMesh->mNormals[i] ^ assimpMesh->mTangents[i]) * assimpMesh->mBitangents[i] < static_cast<ai_real>(0.0))
                    tangent[3u] = static_cast<ai_real>(-1.0);
                vertexBuffer->setVertex(i, reinterpret_cast<const uint8_t*>(tangent.data()));
            }
            mesh->attachVertexBuffer(utils::VertexAttribute::Tangent, vertexBuffer);
        }
    }

    if (assimpMesh->HasTextureCoords(0u))
        mesh->attachVertexBuffer(utils::VertexAttribute::TexCoords, toVertexBuffer(assimpMesh->mNumVertices, assimpMesh->mTextureCoords[0u]));

    if (assimpMesh->HasVertexColors(0u))
        mesh->attachVertexBuffer(utils::VertexAttribute::Color, toVertexBuffer(assimpMesh->mNumVertices, assimpMesh->mColors[0u]));

    if (assimpMesh->HasFaces())
        mesh->attachPrimitiveSet(toDrawElementsBuffer(assimpMesh->mNumFaces, assimpMesh->mFaces));
    else
        mesh->attachPrimitiveSet(toDrawArrays(assimpMesh->mNumVertices));

    if (assimpMesh->HasBones())
    {
        static constexpr uint32_t s_boneComponentsCount = 7u;
        static_assert(s_boneComponentsCount > 0u);

        auto bonesIDsBuffer = std::make_shared<utils::VertexBuffer>(
            assimpMesh->mNumVertices,
            s_boneComponentsCount,
            utils::VertexComponentType::Uint32);
        auto bonesWeightsBuffer = std::make_shared<utils::VertexBuffer>(
            assimpMesh->mNumVertices,
            s_boneComponentsCount,
            utils::toVertexComponentType<ai_real>());

        auto* bonesIDsBufferData = reinterpret_cast<uint32_t*>(bonesIDsBuffer->data());
        auto* bonesWeightsBufferData = reinterpret_cast<ai_real*>(bonesWeightsBuffer->data());
        for (size_t i = 0u; i < static_cast<size_t>(assimpMesh->mNumVertices) * s_boneComponentsCount; ++i)
        {
            bonesIDsBufferData[i] = 0xFFFFFFFFu;
            bonesWeightsBufferData[i] = static_cast<ai_real>(0.0);
        }

        for (uint32_t boneIndex = 0u; boneIndex < assimpMesh->mNumBones; ++boneIndex)
        {
            const auto* assimpBone = assimpMesh->mBones[boneIndex];

            for (uint32_t weightIndex = 0u; weightIndex < assimpBone->mNumWeights; ++weightIndex)
            {
                const auto& assimpWeight = assimpBone->mWeights[weightIndex];
                auto* vertexBonesWeights = bonesWeightsBufferData + assimpWeight.mVertexId * s_boneComponentsCount;
                auto* vertexBonesIDs = bonesIDsBufferData + assimpWeight.mVertexId * s_boneComponentsCount;

                uint32_t boneID = 0u;
                while ((boneID < s_boneComponentsCount) && (vertexBonesWeights[boneID] > assimpWeight.mWeight))
                    ++boneID;

                for (uint32_t t = s_boneComponentsCount - 1u; t > boneID; --t)
                {
                    vertexBonesWeights[t] = vertexBonesWeights[t - 1u];
                    vertexBonesWeights[t] = vertexBonesWeights[t - 1u];
                }

                vertexBonesWeights[boneID] = assimpWeight.mWeight;
                vertexBonesIDs[boneID] = assimpBones.at(assimpBone->mName);
            }
        }

        for (size_t vertexID = 0u; vertexID < static_cast<size_t>(assimpMesh->mNumVertices); ++vertexID)
        {
            auto* vertexBonesWeights = bonesWeightsBufferData + vertexID * s_boneComponentsCount;

            auto weightsSum = static_cast<ai_real>(0.0);
            for (uint32_t i = 0u; i < s_boneComponentsCount; ++i)
                weightsSum += vertexBonesWeights[i];

            if (weightsSum > utils::epsilon<ai_real>())
            {
                for (uint32_t i = 0u; i < s_boneComponentsCount; ++i)
                    vertexBonesWeights[i] /= weightsSum;
            }
        }

        mesh->attachVertexBuffer(utils::VertexAttribute::BonesIDs, bonesIDsBuffer);
        mesh->attachVertexBuffer(utils::VertexAttribute::BonesWeights, bonesWeightsBuffer);
    }

    return std::make_shared<core::Drawable>(
        std::make_shared<core::Mesh>(mesh, toBoundingBox(assimpMesh->mAABB)),
        materials[assimpMesh->mMaterialIndex]);
}

struct Prop
{
    std::string key;
    std::string texSemantic;
    uint32_t texIndex;
    aiPropertyTypeInfo type;
    aiMaterialProperty* prop;

    std::array<float, 4u> floatValue{0.f, 0.f, 0.f, 0.f};
    std::array<double, 4u> doubleValue{0.,0.,0.,0.};
    std::string stringValue{""};
    std::array<int, 4u> intValue{0,0,0,0};
};

struct Tex
{
    unsigned int type;
    std::string typeStr;
    unsigned int index;
    aiString path;
    aiTextureMapping mapping;
    unsigned int uvIndex;
    std::array<aiTextureMapMode, 3u> mapMode;
};

template <typename T>
static aiReturn toMaterialValue(
    const aiMaterial* assimpMaterial,
    const std::initializer_list<std::tuple<const char*, unsigned int, unsigned int>>& types,
    T& resultValue)
{
    aiReturn result = aiReturn_FAILURE;
    for (auto type = types.begin(); (result != aiReturn_SUCCESS) && (type != types.end()); ++type)
    {
        result = assimpMaterial->Get(std::get<0u>(*type), std::get<1u>(*type), std::get<2u>(*type), resultValue);
    }
    return result;
}

static std::shared_ptr<core::MaterialMap> toMaterialMap(
    const aiMaterial* assimpMaterial,
    const std::initializer_list<aiTextureType>& textureTypes,
    const std::filesystem::path& absoluteDir)
{
    std::shared_ptr<core::MaterialMap> materialMap;

    for (auto type = textureTypes.begin(); (!materialMap) && (type != textureTypes.end()); ++type)
    {
        if (uint32_t count = assimpMaterial->GetTextureCount(*type))
        {
            aiString path;
            for (uint32_t i = 0; i < count; ++i)
                if (assimpMaterial->GetTexture(*type, i, &path) == aiReturn_SUCCESS)
                    break;
            materialMap = std::make_shared<simplex::core::MaterialMap>(absoluteDir / toString(path));
        }
    }

    return materialMap;
}

static std::shared_ptr<core::Material> toMaterial(const aiMaterial* assimpMaterial, const std::filesystem::path& absoluteDir)
{
    std::vector<Prop> props;
    for (size_t i = 0u; i < assimpMaterial->mNumProperties; ++i)
    {
        const auto* aiProp = assimpMaterial->mProperties[i];
        Prop p;
        p.key = aiProp->mKey.C_Str();
        p.texIndex = aiProp->mIndex;
        p.texSemantic = aiTextureTypeToString(static_cast<aiTextureType>(aiProp->mSemantic));
        p.type = aiProp->mType;
        p.prop = const_cast<aiMaterialProperty*>(aiProp);

        switch (aiProp->mType)
        {
        case aiPropertyTypeInfo::aiPTI_Float:
        {
            memcpy(p.floatValue.data(), aiProp->mData, aiProp->mDataLength);
            break;
        }
        case aiPropertyTypeInfo::aiPTI_Double:
        {
            memcpy(p.doubleValue.data(), aiProp->mData, aiProp->mDataLength);
            break;
        }
        case aiPropertyTypeInfo::aiPTI_String:
        {
            aiString s;
            s.length = static_cast<unsigned int>(*reinterpret_cast<uint32_t*>(aiProp->mData));
            assert(s.length + 1 + 4 == aiProp->mDataLength);
            assert(!aiProp->mData[aiProp->mDataLength - 1]);
            memcpy(s.data, aiProp->mData + 4, s.length + 1);
            p.stringValue = s.C_Str();
            break;
        }
        case aiPropertyTypeInfo::aiPTI_Integer:
        {
            memcpy(p.intValue.data(), aiProp->mData, aiProp->mDataLength);
            break;
        }
        default:
            break;
        }

        props.push_back(p);
    }

    //tmp
    std::vector<Tex> texs;
    for (unsigned int i = 0u; i <= aiTextureType_GLTF_METALLIC_ROUGHNESS; ++i)
    {
        for (unsigned int ci = 0u; ci < assimpMaterial->GetTextureCount(static_cast<aiTextureType>(i)); ++ci)
        {
            Tex t;
            t.type = i;
            t.typeStr = aiTextureTypeToString(static_cast<aiTextureType>(i));
            t.index = ci;
            assimpMaterial->GetTexture(static_cast<aiTextureType>(i), ci, &t.path, &t.mapping, &t.uvIndex, nullptr, nullptr, t.mapMode.data());
            texs.push_back(t);
        }
    }

    if (!assimpMaterial)
        return nullptr;

    auto material = std::make_shared<core::Material>();

    if (bool materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_TWOSIDED} }, materialValue) == aiReturn_SUCCESS)
        material->setDoubleSided(materialValue);

    glm::vec4 baseColor(1.f);
    if (aiColor4D materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_BASE_COLOR}, {AI_MATKEY_COLOR_DIFFUSE} }, baseColor) == aiReturn_SUCCESS)
        baseColor = toVec4(materialValue);
    if (float materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_OPACITY} }, materialValue) == aiReturn_SUCCESS)
        baseColor.a *= materialValue;
    material->setBaseColor(baseColor);

    if (aiColor3D materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_COLOR_EMISSIVE} }, materialValue) == aiReturn_SUCCESS)
        material->setEmission(toVec3(materialValue));

    if (float materialValue; toMaterialValue(
        assimpMaterial,
        {
            {AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_AMBIENT_OCCLUSION, 0u)},
            {AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_LIGHTMAP, 0u)}
        },
        materialValue) == aiReturn_SUCCESS)
        material->setOcclusionMapStrength(materialValue);

    if (float materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_ROUGHNESS_FACTOR} }, materialValue) == aiReturn_SUCCESS)
        material->setRoughness(materialValue);

    if (float materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_METALLIC_FACTOR} }, materialValue) == aiReturn_SUCCESS)
        material->setMetalness(materialValue);

    if (float materialValue; toMaterialValue(
        assimpMaterial,
        {
            {AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0u)},
            {AI_MATKEY_BUMPSCALING}
        },
        materialValue) == aiReturn_SUCCESS)
        material->setNormalMapScale(materialValue);


    if (aiString materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_GLTF_ALPHAMODE} }, materialValue) == aiReturn_SUCCESS)
    {
        if (materialValue == aiString("MASK"))
        {
            if (float materialValue; toMaterialValue(assimpMaterial, { {AI_MATKEY_GLTF_ALPHACUTOFF} }, materialValue) == aiReturn_SUCCESS)
                material->setAlphaCutoff(materialValue);
        }
    }

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_NORMALS }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::Normal, materialMap);

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_OPACITY }, absoluteDir))
        ;// material->setMaterialMap(core::MaterialMapTarget::Opacity, materialMap);

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_HEIGHT }, absoluteDir))
        ;// material->setMaterialMap(core::MaterialMapTarget::Height, materialMap);

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::Emission, materialMap);

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::BaseColor, materialMap);

    if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_GLTF_METALLIC_ROUGHNESS }, absoluteDir))
    {
        material->setORMSwizzleMask(glm::u32vec3(0u, 1u, 2u));
        material->setMaterialMap(core::MaterialMapTarget::Occlusion, materialMap);
        material->setMaterialMap(core::MaterialMapTarget::Roughness, materialMap);
        material->setMaterialMap(core::MaterialMapTarget::Metalness, materialMap);
    }
    else
    {
        if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Occlusion, materialMap);

        if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_DIFFUSE_ROUGHNESS }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Roughness, materialMap);

        if (auto materialMap = toMaterialMap(assimpMaterial, { aiTextureType_METALNESS }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Metalness, materialMap);
    }

    return material;
}

static size_t toNodeRepresentation(
    const aiNode* assimpNode,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBones,
    std::vector<std::shared_ptr<core::NodeRepresentation>>& nodesRepresentations)
{
    const auto nodeRepresentationName = toString(assimpNode->mName);
    const auto nodeRepresentationTransform = toTransform(assimpNode->mTransformation);

    auto nodeRepresentationID = nodesRepresentations.size();
    nodesRepresentations.push_back(nullptr);

    std::vector<size_t> nodeRepresentationChildrenIDs(assimpNode->mNumChildren);
    for (size_t i = 0u; i < assimpNode->mNumChildren; ++i)
        nodeRepresentationChildrenIDs[i] = toNodeRepresentation(
            assimpNode->mChildren[i],
            assimpBones,
            nodesRepresentations);

    std::shared_ptr<core::NodeRepresentation> nodeRepresentation;

    if (assimpNode->mNumMeshes)
    {
        std::vector<size_t> drawableNodeRepresentationDrawablesIDs(assimpNode->mMeshes, assimpNode->mMeshes + assimpNode->mNumMeshes);
        nodeRepresentation = std::make_shared<core::DrawableNodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs,
            drawableNodeRepresentationDrawablesIDs);
    }
    //else if (auto assimpBoneIter = assimpBones.find(assimpNode->mName); assimpBoneIter != assimpBones.end())
    //{

    //}
    else
    {
        nodeRepresentation = std::make_shared<core::NodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs);
    }

    nodesRepresentations[nodeRepresentationID] = nodeRepresentation;
    return nodeRepresentationID;
}

static std::shared_ptr<core::AnimationChannel> toAnimationChannel(
    const aiNodeAnim* assimpNodeAnim,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBones)
{
    std::vector<std::pair<uint32_t, float>> scaleKeys(assimpNodeAnim->mNumScalingKeys);
    for (size_t i = 0u; i < assimpNodeAnim->mNumScalingKeys; ++i)
        scaleKeys[i] = {
            static_cast<uint32_t>(assimpNodeAnim->mScalingKeys[i].mTime + 0.5),
            static_cast<float>(assimpNodeAnim->mScalingKeys[i].mValue.x) };

    std::vector<std::pair<uint32_t, glm::quat>> rotationKeys(assimpNodeAnim->mNumRotationKeys);
    for (size_t i = 0u; i < assimpNodeAnim->mNumRotationKeys; ++i)
        rotationKeys[i] = {
            static_cast<uint32_t>(assimpNodeAnim->mRotationKeys[i].mTime + 0.5),
            toQuat(assimpNodeAnim->mRotationKeys[i].mValue) };

    std::vector<std::pair<uint32_t, glm::vec3>> translationKeys(assimpNodeAnim->mNumPositionKeys);
    for (size_t i = 0u; i < assimpNodeAnim->mNumPositionKeys; ++i)
        translationKeys[i] = {
            static_cast<uint32_t>(assimpNodeAnim->mPositionKeys[i].mTime + 0.5),
            toVec3(assimpNodeAnim->mPositionKeys[i].mValue) };

    auto animationChannel = std::make_shared<core::AnimationChannel>();
    animationChannel->setBoneID(assimpBones.at(assimpNodeAnim->mNodeName));
    animationChannel->setScaleKeys(scaleKeys);
    animationChannel->setRotationKeys(rotationKeys);
    animationChannel->setTranslationKeys(translationKeys);
    return animationChannel;
}

static std::shared_ptr<core::Animation> toAnimation(
    const aiAnimation* assimpAnimation,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBones)
{
    if (!assimpAnimation)
        return nullptr;

    std::vector<std::shared_ptr<core::AnimationChannel>> animationChannels;
    animationChannels.reserve(assimpAnimation->mNumChannels);
    for (size_t i = 0u; i < assimpAnimation->mNumChannels; ++i)
    {
        animationChannels.push_back(toAnimationChannel(assimpAnimation->mChannels[i], assimpBones));
    }

    auto animation = std::make_shared<core::Animation>(toString(assimpAnimation->mName));
    animation->setDuration(static_cast<uint32_t>(assimpAnimation->mDuration + 0.5));
    animation->setTicksPerSecond(static_cast<uint32_t>(assimpAnimation->mDuration + 0.5));
    animation->setChannels(animationChannels);

    return animation;
}

std::shared_ptr<core::SceneRepresentation> LOADER_ASSIMP_GLOBAL_SHARED_EXPORT load(const std::filesystem::path& filename)
{
    const auto absoluteFileName = std::filesystem::absolute(filename);
    const auto absoluteDir = absoluteFileName.parent_path();

    Assimp::Importer importer;
    const auto* assimpScene = importer.ReadFile(
        absoluteFileName.string(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);

    if (!assimpScene)
    {
        LOG_ERROR << "Can't open scene file " << filename;
        return nullptr;
    }

    std::vector<std::shared_ptr<core::Material>> materials;
    if (assimpScene->HasMaterials())
    {
        materials.reserve(assimpScene->mNumMaterials);
        for (uint32_t i = 0u; i < assimpScene->mNumMaterials; ++i)
        {
            materials.push_back(toMaterial(assimpScene->mMaterials[i], absoluteDir));
        }
    }

    std::unordered_map<aiString, uint32_t, aiStringHash> assimpBones;
    std::vector<utils::Transform> bones;
    std::vector<std::shared_ptr<core::Drawable>> drawables;
    if (assimpScene->HasMeshes())
    {
        for (size_t meshIndex = 0u; meshIndex < assimpScene->mNumMeshes; ++meshIndex)
        {
            const auto* assimpMesh = assimpScene->mMeshes[meshIndex];
            if (assimpMesh->HasBones())
            {
                for (uint32_t boneIndex = 0u; boneIndex < assimpMesh->mNumBones; ++boneIndex)
                {
                    const auto* assimpBone = assimpMesh->mBones[boneIndex];
                    if (assimpBones.find(assimpBone->mName) == assimpBones.end())
                    {
                        auto boneID = static_cast<uint32_t>(bones.size());
                        bones.push_back(toTransform(assimpBone->mOffsetMatrix));

                        assimpBones.insert({ assimpBone->mName, boneID });
                    }
                }
            }
        }

        drawables.reserve(assimpScene->mNumMeshes);
        for (uint32_t i = 0u; i < assimpScene->mNumMeshes; ++i)
        {
            drawables.push_back(toDrawable(assimpScene->mMeshes[i], assimpBones, materials));
        }
    }

    std::vector<std::shared_ptr<core::Animation>> animations;
    if (assimpScene->HasAnimations())
    {
        animations.reserve(assimpScene->mNumAnimations);
        for (size_t i = 0u; i < assimpScene->mNumAnimations; ++i)
            animations.push_back(toAnimation(assimpScene->mAnimations[i], assimpBones));
    }

    std::vector<std::shared_ptr<core::NodeRepresentation>> nodesRepresentations;
    auto rootNodeRepresentationID = toNodeRepresentation(assimpScene->mRootNode, assimpBones, nodesRepresentations);

    return std::make_shared<core::SceneRepresentation>(
        toString(assimpScene->mName),
        drawables,
        nodesRepresentations,
        animations,
        rootNodeRepresentationID);
}

}
}