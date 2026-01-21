#include <loader_assimp/loader.h>

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/boundingbox.h>
#include <utils/mesh.h>
#include <utils/logger.h>

#include <core/drawable.h>
#include <core/drawablenode.h>
#include <core/material.h>
#include <core/mesh.h>

namespace simplex
{
namespace loader_assimp
{

using AssimpVerticesCount = decltype(aiMesh::mNumVertices);
using AssimpFacesCount = decltype(aiMesh::mNumFaces);
using AssimpMaterialIndex = decltype(aiMesh::mMaterialIndex);

using AssimpFaceIndex = std::remove_pointer<decltype(aiFace::mIndices)>::type;
static const auto s_drawElementsIndexType = utils::toDrawElementsIndexType<AssimpFaceIndex>();
static const auto s_triangleSize = 3u * utils::DrawElements::indexSize(s_drawElementsIndexType);

using MaterialsVector = std::vector<std::shared_ptr<core::Material>>;

using MeshAndMaterialIndex = std::pair<std::shared_ptr<core::Mesh>, AssimpMaterialIndex>;
using MeshesVector = std::vector<MeshAndMaterialIndex>;

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
static std::shared_ptr<utils::VertexBuffer> toVertexBuffer(AssimpVerticesCount verticesCount, const aiVector3t<TReal>* data)
{
    return toVertexBuffer<3u, TReal>(static_cast<size_t>(verticesCount), reinterpret_cast<const void*>(data));
}

static std::shared_ptr<utils::VertexBuffer> toVertexBuffer(AssimpVerticesCount verticesCount, const aiColor4t<float>* data)
{
    return toVertexBuffer<4u, float>(static_cast<size_t>(verticesCount), reinterpret_cast<const void*>(data));
}

static std::shared_ptr<utils::DrawElementsBuffer> toDrawElementsBuffer(AssimpFacesCount facesCount, const aiFace* data)
{
    auto drawElementsBuffer = std::make_shared<utils::DrawElementsBuffer>(
        utils::PrimitiveType::Triangles,
        3u * static_cast<size_t>(facesCount),
        s_drawElementsIndexType,
        0u);

    for (size_t i = 0u; i < facesCount; ++i)
        memcpy(drawElementsBuffer->data() + s_triangleSize * i, data[i].mIndices, s_triangleSize);

    return drawElementsBuffer;
}

static std::shared_ptr<utils::DrawArrays> toDrawArrays(AssimpVerticesCount verticesCount)
{
    return std::make_shared<utils::DrawArrays>(utils::PrimitiveType::Triangles, 0u, static_cast<size_t>(verticesCount));
}

static MeshAndMaterialIndex toMesh(const aiMesh* value)
{
    if (!value)
        return { nullptr, 0u };

    auto mesh = std::make_shared<utils::Mesh>();

    if (value->HasPositions())
        mesh->attachVertexBuffer(utils::VertexAttribute::Position, toVertexBuffer(value->mNumVertices, value->mVertices));

    if (value->HasNormals())
    {
        mesh->attachVertexBuffer(utils::VertexAttribute::Normal, toVertexBuffer(value->mNumVertices, value->mNormals));

        if (value->HasTangentsAndBitangents())
        {
            auto vertexBuffer = std::make_shared<utils::VertexBuffer>(value->mNumVertices, 4u, utils::toVertexComponentType<ai_real>());
            for (size_t i = 0u; i < value->mNumVertices; ++i)
            {
                std::array<ai_real, 4u> tangent {
                    value->mTangents[i][0u],
                    value->mTangents[i][1u],
                    value->mTangents[i][2u],
                    static_cast<ai_real>(1.0) };
                if ((value->mNormals[i] ^ value->mTangents[i]) * value->mBitangents[i] < static_cast<ai_real>(0.0))
                    tangent[3u] = static_cast<ai_real>(-1.0);
                vertexBuffer->setVertex(i, reinterpret_cast<const uint8_t*>(tangent.data()));
            }
            mesh->attachVertexBuffer(utils::VertexAttribute::Tangent, vertexBuffer);
        }
    }

    if (value->HasTextureCoords(0u))
        mesh->attachVertexBuffer(utils::VertexAttribute::TexCoords, toVertexBuffer(value->mNumVertices, value->mTextureCoords[0u]));

    if (value->HasVertexColors(0u))
        mesh->attachVertexBuffer(utils::VertexAttribute::Color, toVertexBuffer(value->mNumVertices, value->mColors[0u]));

    if (value->HasFaces())
        mesh->attachPrimitiveSet(toDrawElementsBuffer(value->mNumFaces, value->mFaces));
    else
        mesh->attachPrimitiveSet(toDrawArrays(value->mNumVertices));

    return { std::make_shared<core::Mesh>(mesh, toBoundingBox(value->mAABB)), value->mMaterialIndex};
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
    const aiMaterial* value,
    const std::initializer_list<std::tuple<const char*, unsigned int, unsigned int>>& types,
    T& resultValue)
{
    aiReturn result = aiReturn_FAILURE;
    for (auto type = types.begin(); (result != aiReturn_SUCCESS) && (type != types.end()); ++type)
    {
        result = value->Get(std::get<0u>(*type), std::get<1u>(*type), std::get<2u>(*type), resultValue);
    }
    return result;
}

static std::shared_ptr<core::MaterialMap> toMaterialMap(
    const aiMaterial* value,
    const std::initializer_list<aiTextureType>& textureTypes,
    const std::filesystem::path& absoluteDir)
{
    std::shared_ptr<core::MaterialMap> materialMap;

    for (auto type = textureTypes.begin(); (!materialMap) && (type != textureTypes.end()); ++type)
    {
        if (uint32_t count = value->GetTextureCount(*type))
        {
            aiString path;
            for (uint32_t i = 0; i < count; ++i)
                if (value->GetTexture(*type, i, &path) == aiReturn_SUCCESS)
                    break;
            materialMap = std::make_shared<simplex::core::MaterialMap>(absoluteDir / toString(path));
        }
    }

    return materialMap;
}

static std::shared_ptr<core::Material> toMaterial(const aiMaterial* value, const std::filesystem::path& absoluteDir)
{
    std::vector<Prop> props;
    for (size_t i = 0u; i < value->mNumProperties; ++i)
    {
        const auto* aiProp = value->mProperties[i];
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
        for (unsigned int ci = 0u; ci < value->GetTextureCount(static_cast<aiTextureType>(i)); ++ci)
        {
            Tex t;
            t.type = i;
            t.typeStr = aiTextureTypeToString(static_cast<aiTextureType>(i));
            t.index = ci;
            value->GetTexture(static_cast<aiTextureType>(i), ci, &t.path, &t.mapping, &t.uvIndex, nullptr, nullptr, t.mapMode.data());
            texs.push_back(t);
        }
    }

    if (!value)
        return nullptr;

    auto material = std::make_shared<core::Material>();

    if (bool materialValue; toMaterialValue(value, { {AI_MATKEY_TWOSIDED} }, materialValue) == aiReturn_SUCCESS)
        material->setDoubleSided(materialValue);

    glm::vec4 baseColor(1.f);
    if (aiColor4D materialValue; toMaterialValue(value, { {AI_MATKEY_BASE_COLOR}, {AI_MATKEY_COLOR_DIFFUSE} }, baseColor) == aiReturn_SUCCESS)
        baseColor = toVec4(materialValue);
    if (float materialValue; toMaterialValue(value, { {AI_MATKEY_OPACITY} }, materialValue) == aiReturn_SUCCESS)
        baseColor.a *= materialValue;
    material->setBaseColor(baseColor);

    if (aiColor3D materialValue; toMaterialValue(value, { {AI_MATKEY_COLOR_EMISSIVE} }, materialValue) == aiReturn_SUCCESS)
        material->setEmission(toVec3(materialValue));

    if (float materialValue; toMaterialValue(
        value,
        {
            {AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_AMBIENT_OCCLUSION, 0u)},
            {AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_LIGHTMAP, 0u)}
        },
        materialValue) == aiReturn_SUCCESS)
        material->setOcclusionMapStrength(materialValue);

    if (float materialValue; toMaterialValue(value, { {AI_MATKEY_ROUGHNESS_FACTOR} }, materialValue) == aiReturn_SUCCESS)
        material->setRoughness(materialValue);

    if (float materialValue; toMaterialValue(value, { {AI_MATKEY_METALLIC_FACTOR} }, materialValue) == aiReturn_SUCCESS)
        material->setMetalness(materialValue);

    if (float materialValue; toMaterialValue(
        value,
        {
            {AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0u)},
            {AI_MATKEY_BUMPSCALING}
        },
        materialValue) == aiReturn_SUCCESS)
        material->setNormalMapScale(materialValue);


    if (aiString materialValue; toMaterialValue(value, { {AI_MATKEY_GLTF_ALPHAMODE} }, materialValue) == aiReturn_SUCCESS)
    {
        if (materialValue == aiString("MASK"))
        {
            if (float materialValue; toMaterialValue(value, { {AI_MATKEY_GLTF_ALPHACUTOFF} }, materialValue) == aiReturn_SUCCESS)
                material->setAlphaCutoff(materialValue);
        }
    }

    if (auto materialMap = toMaterialMap(value, { aiTextureType_NORMALS }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::Normal, materialMap);

    if (auto materialMap = toMaterialMap(value, { aiTextureType_OPACITY }, absoluteDir))
        ;// material->setMaterialMap(core::MaterialMapTarget::Opacity, materialMap);

    if (auto materialMap = toMaterialMap(value, { aiTextureType_HEIGHT }, absoluteDir))
        ;// material->setMaterialMap(core::MaterialMapTarget::Height, materialMap);

    if (auto materialMap = toMaterialMap(value, { aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::Emission, materialMap);

    if (auto materialMap = toMaterialMap(value, { aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE }, absoluteDir))
        material->setMaterialMap(core::MaterialMapTarget::BaseColor, materialMap);

    if (auto materialMap = toMaterialMap(value, { aiTextureType_GLTF_METALLIC_ROUGHNESS }, absoluteDir))
    {
        material->setORMSwizzleMask(glm::u32vec3(0u, 1u, 2u));
        material->setMaterialMap(core::MaterialMapTarget::Occlusion, materialMap);
        material->setMaterialMap(core::MaterialMapTarget::Roughness, materialMap);
        material->setMaterialMap(core::MaterialMapTarget::Metalness, materialMap);
    }
    else
    {
        if (auto materialMap = toMaterialMap(value, { aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Occlusion, materialMap);

        if (auto materialMap = toMaterialMap(value, { aiTextureType_DIFFUSE_ROUGHNESS }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Roughness, materialMap);

        if (auto materialMap = toMaterialMap(value, { aiTextureType_METALNESS }, absoluteDir))
            material->setMaterialMap(core::MaterialMapTarget::Metalness, materialMap);
    }

    return material;
}

static std::shared_ptr<core::Node> toNode(
    const aiNode* value,
    const MeshesVector& meshes,
    const MaterialsVector& materials)
{
    if (!value)
        return nullptr;

    std::shared_ptr<core::Node> node;

    if (value->mNumMeshes)
    {
        auto drawableNode = std::make_shared<core::DrawableNode>(toString(value->mName));

        for (size_t i = 0u; i < value->mNumMeshes; ++i)
        {
            const auto& [mesh, materialIndex] = meshes[value->mMeshes[i]];
            drawableNode->addDrawable(std::make_shared<core::Drawable>(mesh, materials[materialIndex]));
        }

        node = drawableNode;
    }
    else
    {
        node = std::make_shared<core::Node>(toString(value->mName));
    }

    node->setTransform(toTransform(value->mTransformation));

    for (size_t i = 0u; i < value->mNumChildren; ++i)
        node->attach(toNode(value->mChildren[i], meshes, materials));

    return node;
}

std::shared_ptr<core::Node> LOADER_ASSIMP_GLOBAL_SHARED_EXPORT load(const std::filesystem::path& filename)
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

    MeshesVector meshes;
    if (assimpScene->HasMeshes())
    {
        meshes.reserve(assimpScene->mNumMeshes);
        for (uint32_t i = 0u; i < assimpScene->mNumMeshes; ++i)
        {
            const auto* assimpMesh = assimpScene->mMeshes[i];
            meshes.push_back(toMesh(assimpMesh));
        }
    }

    MaterialsVector materials;
    if (assimpScene->HasMaterials())
    {
        materials.reserve(assimpScene->mNumMaterials);
        for (uint32_t i = 0u; i < assimpScene->mNumMaterials; ++i)
        {
            materials.push_back(toMaterial(assimpScene->mMaterials[i], absoluteDir));
        }
    }

    return toNode(assimpScene->mRootNode, meshes, materials);
}

}
}