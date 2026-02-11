#include <unordered_map>

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/boundingbox.h>
#include <utils/clipspace.h>
#include <utils/mesh.h>
#include <utils/logger.h>

#include <core/skeletalanimation.h>
#include <core/drawable.h>
#include <core/material.h>
#include <core/mesh.h>

#include <scenes_loader_assimp/scenesloaderassimp.h>

namespace simplex
{

namespace scenes_loader_assimp
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

static utils::PrimitiveType toPrimitiveType(aiPrimitiveType assimpPrimitiveType)
{
    utils::PrimitiveType result = utils::PrimitiveType::Count;
    switch (assimpPrimitiveType)
    {
    case aiPrimitiveType_POINT: { result = utils::PrimitiveType::Points; break; }
    case aiPrimitiveType_LINE: { result = utils::PrimitiveType::Lines; break; }
    case aiPrimitiveType_TRIANGLE: { result = utils::PrimitiveType::Triangles; break; }
    default: { break; }
    }
    return result;
}

static uint32_t toPrimitiveIndicesCount(aiPrimitiveType assimpPrimitiveType)
{
    uint32_t result = 0u;
    switch (assimpPrimitiveType)
    {
    case aiPrimitiveType_POINT: { result = 1u; break; }
    case aiPrimitiveType_LINE: { result = 2u; break; }
    case aiPrimitiveType_TRIANGLE: { result = 3u; break; }
    default: { break; }
    }
    return result;
}

static std::shared_ptr<utils::DrawElementsBuffer> toDrawElementsBuffer(
    aiPrimitiveType assimpPrimitiveType,
    size_t facesCount,
    const aiFace* data)
{
    const auto primitiveTypeIndicesCount = toPrimitiveIndicesCount(assimpPrimitiveType);

    auto drawElementsBuffer = std::make_shared<utils::DrawElementsBuffer>(
        toPrimitiveType(assimpPrimitiveType),
        primitiveTypeIndicesCount * facesCount,
        s_drawElementsIndexType,
        0u);

    const auto faceSize = primitiveTypeIndicesCount * utils::DrawElements::indexSize(s_drawElementsIndexType);
    for (size_t i = 0u; i < facesCount; ++i)
        memcpy(drawElementsBuffer->data() + faceSize * i, data[i].mIndices, faceSize);

    return drawElementsBuffer;
}

static std::shared_ptr<utils::DrawArrays> toDrawArrays(
    aiPrimitiveType assimpPrimitiveType,
    size_t verticesCount)
{
    return std::make_shared<utils::DrawArrays>(
        toPrimitiveType(assimpPrimitiveType),
        0u,
        verticesCount);
}

static std::shared_ptr<core::Drawable> toDrawable(
    const aiMesh* assimpMesh,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs,
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

    if (assimpMesh->HasBones())
    {
        static constexpr uint32_t s_boneComponentsCount = 4u;
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

                if (boneID >= s_boneComponentsCount)
                    continue;

                for (uint32_t t = s_boneComponentsCount - 1u; t > boneID; --t)
                {
                    vertexBonesWeights[t] = vertexBonesWeights[t - 1u];
                    vertexBonesIDs[t] = vertexBonesIDs[t - 1u];
                }

                vertexBonesWeights[boneID] = assimpWeight.mWeight;
                vertexBonesIDs[boneID] = assimpBonesIDs.at(assimpBone->mName);
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

    if (assimpMesh->HasVertexColors(0u))
        mesh->attachVertexBuffer(utils::VertexAttribute::Color, toVertexBuffer(assimpMesh->mNumVertices, assimpMesh->mColors[0u]));


    // All of faces have the same primitive type becuse of aiProcess_SortByPType flag during loading the scene
    const auto primitiveType = static_cast<aiPrimitiveType>(assimpMesh->mPrimitiveTypes);

    if (assimpMesh->HasFaces())
    {
        // assimpMesh->mPrimitiveTypes didn't have an unique aiPrimitiveType for some case
        const_cast<aiPrimitiveType>(primitiveType) = AI_PRIMITIVE_TYPE_FOR_N_INDICES(assimpMesh->mFaces[0u].mNumIndices);

        mesh->attachPrimitiveSet(toDrawElementsBuffer(
            primitiveType,
            assimpMesh->mNumFaces,
            assimpMesh->mFaces));
    }
    else
    {
        mesh->attachPrimitiveSet(toDrawArrays(
            primitiveType,
            assimpMesh->mNumVertices));
    }

    return std::make_shared<core::Drawable>(
        std::make_shared<core::Mesh>(mesh, toBoundingBox(assimpMesh->mAABB)),
        materials[assimpMesh->mMaterialIndex]);
}

template <typename T>
static aiReturn toMaterialValue(
    const aiMaterial* assimpMaterial,
    const std::initializer_list<std::tuple<const char*, unsigned int, unsigned int>>& assimpTypes,
    T& resultValue)
{
    aiReturn result = aiReturn_FAILURE;
    for (auto type = assimpTypes.begin(); (result != aiReturn_SUCCESS) && (type != assimpTypes.end()); ++type)
    {
        result = assimpMaterial->Get(std::get<0u>(*type), std::get<1u>(*type), std::get<2u>(*type), resultValue);
    }
    return result;
}

static std::shared_ptr<core::MaterialMap> toMaterialMap(
    const aiMaterial* assimpMaterial,
    const std::initializer_list<aiTextureType>& assimpTextureTypes,
    const std::filesystem::path& absoluteDir)
{
    std::shared_ptr<core::MaterialMap> materialMap;

    for (auto type = assimpTextureTypes.begin(); (!materialMap) && (type != assimpTextureTypes.end()); ++type)
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
        material->setMaterialMap(core::MaterialMapTarget::Opacity, materialMap);

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

static core::AnimationChannel toAnimationChannel(
    const aiNodeAnim* assimpNodeAnim,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs)
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

    return {
        assimpBonesIDs.at(assimpNodeAnim->mNodeName),
        scaleKeys,
        rotationKeys,
        translationKeys };
}

static std::shared_ptr<core::Animation> toAnimation(
    const aiAnimation* assimpAnimation,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs)
{
    std::vector<core::AnimationChannel> animationChannels;
    animationChannels.reserve(assimpAnimation->mNumChannels);
    for (size_t i = 0u; i < assimpAnimation->mNumChannels; ++i)
    {
        animationChannels.push_back(toAnimationChannel(assimpAnimation->mChannels[i], assimpBonesIDs));
    }

    const auto duration = static_cast<uint32_t>(assimpAnimation->mDuration + .5);
    const auto ticksPerSecond = static_cast<uint32_t>(assimpAnimation->mTicksPerSecond > 0. ?
        assimpAnimation->mTicksPerSecond + .5 :
        25.);

    return std::make_shared<core::Animation>(duration, ticksPerSecond, animationChannels);
}

static auto assimpFindParentBoneNode = [](
    const aiNode* assimpBoneNode,
    const std::unordered_map<aiString, const aiNode*, aiStringHash>& assimpBoneNodes)
{
    const aiNode* result = nullptr;

    for (const auto* assimpParentNode = assimpBoneNode->mParent;
        !result && assimpParentNode;
        assimpParentNode = assimpParentNode->mParent)
    {
        if (auto it = assimpBoneNodes.find(assimpParentNode->mName); it != assimpBoneNodes.end())
            result = it->second;
    }

    return result;
};

static auto assimpFindRootBoneNode = [](
    const aiNode* assimpBoneNode,
    const std::unordered_map<aiString, const aiNode*, aiStringHash>& assimpBoneNodes)
{
    const aiNode* result = nullptr;

    for (const auto* assimpParentBoneNode = assimpBoneNode;
        assimpParentBoneNode;
        assimpParentBoneNode = assimpFindParentBoneNode(result, assimpBoneNodes))
    {
        result = assimpParentBoneNode;
    }

    return result;
};

static std::vector<std::shared_ptr<core::Skeleton>> toSkeletons(
    const aiScene* assimpScene,
    const std::unordered_map<aiString, const aiNode*, aiStringHash>& assimpBoneNodes,
    std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs,
    std::unordered_map<aiString, uint32_t, aiStringHash>& assimpSkeletonsIDs)
{
    static auto assimpBoneTransform = [](const aiNode* assimpBoneNode, const aiNode* assimpParentBoneNode)
    {
        utils::Transform result;
        for (const auto* assimpParentNode = assimpBoneNode;
            assimpParentNode && (assimpParentNode != assimpParentBoneNode);
            assimpParentNode = assimpParentNode->mParent)
            result = result * toTransform(assimpParentNode->mTransformation);
        return result;
    };

    static auto assimpFindBoneID = [](
        auto& self,
        const aiScene* assimpScene,
        const aiNode* assimpBoneNode,
        const std::unordered_map<aiString, const aiNode*, aiStringHash>& assimpBoneNodes,
        std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs,
        std::vector<core::Bone>& bones)
    {
        if (!assimpBoneNode)
            return 0xFFFFFFFFu;

        if (auto it = assimpBonesIDs.find(assimpBoneNode->mName); it != assimpBonesIDs.end())
            return it->second;

        const auto* assimpParentBoneNode = assimpFindParentBoneNode(assimpBoneNode, assimpBoneNodes);

        const uint32_t parentBoneID = self(
            self,
            assimpScene,
            assimpParentBoneNode,
            assimpBoneNodes,
            assimpBonesIDs,
            bones);

        const uint32_t boneID = static_cast<uint32_t>(bones.size());
        bones.push_back({
            toTransform(assimpScene->findBone(assimpBoneNode->mName)->mOffsetMatrix),
            assimpBoneTransform(assimpBoneNode, assimpParentBoneNode),
            parentBoneID,
            std::vector<uint32_t>() });

        assimpBonesIDs.insert({ assimpBoneNode->mName, boneID });

        return boneID;
    };

    std::vector<std::tuple<std::vector<core::Bone>, uint32_t, std::map<std::string, std::shared_ptr<core::Animation>>>> skeletons;
    for (const auto& [_, assimpBoneNode] : assimpBoneNodes)
    {
        const auto* assimpRootBoneNode = assimpFindRootBoneNode(assimpBoneNode, assimpBoneNodes);
        if (auto it = assimpSkeletonsIDs.find(assimpRootBoneNode->mName); it == assimpSkeletonsIDs.end())
        {
            auto skeletonID = static_cast<uint32_t>(skeletons.size());
            skeletons.push_back(std::make_tuple(
                std::vector<core::Bone>(),
                0xFFFFFFFFu,
                std::map<std::string, std::shared_ptr<core::Animation>>()));
            assimpSkeletonsIDs.insert({ assimpRootBoneNode->mName, skeletonID });
        }
    }

    for (const auto& [_, assimpBoneNode] : assimpBoneNodes)
    {
        const auto* assimpRootBoneNode = assimpFindRootBoneNode(assimpBoneNode, assimpBoneNodes);
        const auto skeletonID = assimpSkeletonsIDs.at(assimpRootBoneNode->mName);
        auto& skeletonBones = std::get<0>(skeletons[skeletonID]);

        const auto boneID = assimpFindBoneID(
            assimpFindBoneID,
            assimpScene,
            assimpBoneNode,
            assimpBoneNodes,
            assimpBonesIDs,
            skeletonBones);

        if (const auto parentBoneID = skeletonBones[boneID].parentID; parentBoneID != 0xFFFFFFFFu)
            skeletonBones[parentBoneID].childrenIDs.push_back(boneID);
        else
            std::get<1>(skeletons[skeletonID]) = boneID;
    }

    if (assimpScene->HasAnimations())
    {
        for (uint32_t i = 0u; i < assimpScene->mNumAnimations; ++i)
        {
            const auto* assimpAnimation = assimpScene->mAnimations[i];
            if (assimpAnimation->mNumChannels && assimpAnimation->mChannels)
            {
                if (auto boneIt = assimpBoneNodes.find(assimpAnimation->mChannels[0u]->mNodeName); boneIt != assimpBoneNodes.end())
                {
                    const auto* assimpRootBoneNode = assimpFindRootBoneNode(boneIt->second, assimpBoneNodes);
                    if (auto rootBoneIt = assimpSkeletonsIDs.find(assimpRootBoneNode->mName); rootBoneIt != assimpSkeletonsIDs.end())
                    {
                        const auto skeletonID = rootBoneIt->second;
                        auto& skeletonAnimations = std::get<2>(skeletons[skeletonID]);
                        skeletonAnimations.insert({
                            toString(assimpAnimation->mName),
                            toAnimation(assimpAnimation, assimpBonesIDs)});
                    }
                }
                
            }
        }
    }

    std::vector<std::shared_ptr<core::Skeleton>> result;
    for (const auto& skeleton : skeletons)
        result.push_back(std::make_shared<core::Skeleton>(std::get<0>(skeleton), std::get<1>(skeleton), std::get<2>(skeleton)));

    return result;
}

static size_t toNodeRepresentation(
    const aiNode* assimpNode,
    const std::unordered_map<aiString, const aiNode*, aiStringHash>& assimpBoneNodes,
    const std::unordered_map<aiString, const aiCamera*, aiStringHash>& assimpCameras,
    const std::unordered_map<aiString, const aiLight*, aiStringHash>& assimpLights,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpBonesIDs,
    const std::unordered_map<aiString, uint32_t, aiStringHash>& assimpSkeletonsIDs,
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
            assimpBoneNodes,
            assimpCameras,
            assimpLights,
            assimpBonesIDs,
            assimpSkeletonsIDs,
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
    else if (auto assimpRootBoneIter = assimpSkeletonsIDs.find(assimpNode->mName); assimpRootBoneIter != assimpSkeletonsIDs.end())
    {
        nodeRepresentation = std::make_shared<core::RootBoneNodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs,
            assimpRootBoneIter->second,
            assimpBonesIDs.at(assimpNode->mName));
    }
    else if (auto assimpBoneIter = assimpBonesIDs.find(assimpNode->mName); assimpBoneIter != assimpBonesIDs.end())
    {
        nodeRepresentation = std::make_shared<core::BoneNodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs,
            assimpBoneIter->second);
    }
    else if (auto assimpCameraIter = assimpCameras.find(assimpNode->mName); assimpCameraIter != assimpCameras.end())
    {
        const auto cs = utils::ClipSpace::makeIdentity();
        const auto z = utils::Range();
        nodeRepresentation = std::make_shared<core::CameraNodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs,
            cs,
            z);
    }
    else if (auto assimpLightIter = assimpLights.find(assimpNode->mName); assimpLightIter != assimpLights.end())
    {
        nodeRepresentation = std::make_shared<core::LightNodeRepresentation>(
            nodeRepresentationName,
            nodeRepresentationTransform,
            nodeRepresentationChildrenIDs);
    }
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

AssimpScenesLoader::AssimpScenesLoader()
    : core::LoaderBase<core::SceneRepresentation>("AssimpScenesLoader")
{}

AssimpScenesLoader::~AssimpScenesLoader() = default;

std::shared_ptr<core::SceneRepresentation> AssimpScenesLoader::loadResource(const std::filesystem::path& absoluteFileName) const
{
    const auto absoluteDir = absoluteFileName.parent_path();

    Assimp::Importer importer;
    const auto* assimpScene = importer.ReadFile(
        absoluteFileName.string(),
        aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);

    if (!assimpScene)
        return nullptr;

    std::vector<std::shared_ptr<core::Material>> materials;
    if (assimpScene->HasMaterials())
    {
        materials.reserve(assimpScene->mNumMaterials);
        for (uint32_t i = 0u; i < assimpScene->mNumMaterials; ++i)
        {
            materials.push_back(toMaterial(assimpScene->mMaterials[i], absoluteDir));
        }
    }

    std::unordered_map<aiString, const aiCamera*, aiStringHash> assimpCameras;
    if (assimpScene->HasCameras())
    {
        assimpCameras.reserve(assimpScene->mNumCameras);
        for (uint32_t i = 0u; i < assimpScene->mNumCameras; ++i)
        {
            auto* assimpCamera = assimpScene->mCameras[i];
            assimpCameras.insert({ assimpCamera->mName, assimpCamera });
        }
    }

    std::unordered_map<aiString, const aiLight*, aiStringHash> assimpLights;
    if (assimpScene->HasLights())
    {
        assimpLights.reserve(assimpScene->mNumLights);
        for (uint32_t i = 0u; i < assimpScene->mNumLights; ++i)
        {
            auto* assimpLight = assimpScene->mLights[i];
            assimpLights.insert({ assimpLight->mName, assimpLight });
        }
    }

    std::vector<std::shared_ptr<core::Drawable>> drawables;
    std::vector<std::shared_ptr<core::Skeleton>> skeletons;
    std::unordered_map<aiString, const aiNode*, aiStringHash> assimpBoneNodes;
    std::unordered_map<aiString, uint32_t, aiStringHash> assimpBonesIDs;
    std::unordered_map<aiString, uint32_t, aiStringHash> assimpSkeletonsIDs;
    if (assimpScene->HasMeshes())
    {
        for (uint32_t i = 0u; i < assimpScene->mNumMeshes; ++i)
        {
            const auto* assimpMesh = assimpScene->mMeshes[i];
            if (assimpMesh->HasBones())
            {
                for (uint32_t j = 0u; j < assimpMesh->mNumBones; ++j)
                {
                    auto* assimpBone = assimpMesh->mBones[j];
                    if (auto assimpBoneNodeIter = assimpBoneNodes.find(assimpBone->mName); assimpBoneNodeIter == assimpBoneNodes.end())
                    {
                        assimpBoneNodes.insert({
                            assimpBone->mName,
                            assimpScene->mRootNode->findBoneNode(assimpBone) });
                    }
                }
            }
        }

        skeletons = toSkeletons(assimpScene, assimpBoneNodes, assimpBonesIDs, assimpSkeletonsIDs);

        drawables.reserve(assimpScene->mNumMeshes);
        for (uint32_t i = 0u; i < assimpScene->mNumMeshes; ++i)
        {
            auto* assimpMesh = assimpScene->mMeshes[i];
            drawables.push_back(toDrawable(assimpMesh, assimpBonesIDs, materials));
        }
    }

    std::vector<std::shared_ptr<core::NodeRepresentation>> nodesRepresentations;
    const auto rootNodeRepresentationID = toNodeRepresentation(
        assimpScene->mRootNode,
        assimpBoneNodes,
        assimpCameras,
        assimpLights,
        assimpBonesIDs,
        assimpSkeletonsIDs,
        nodesRepresentations);

    return std::make_shared<core::SceneRepresentation>(
        toString(assimpScene->mName),
        drawables,
        skeletons,
        nodesRepresentations,
        rootNodeRepresentationID);
}

}
}