#ifndef CORE_DESCRIPTIONS_H
#define CORE_DESCRIPTIONS_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/frustum.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace core
{

struct TransformDescription
{
    glm::vec4 rotation;
    glm::vec4 translationAndScale;

    static TransformDescription makeEmpty();
    static TransformDescription make(const utils::Transform&);
};

struct CameraDescription
{
    TransformDescription viewTransform;
    TransformDescription viewTransformInverted;
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 projectionMatrixInverted;
    glm::mat4x4 viewProjectionMatrix;
    glm::mat4x4 viewProjectionMatrixInverted;
    glm::vec4 viewPosition;
    glm::vec4 viewXDirection;
    glm::vec4 viewYDirection;
    glm::vec4 viewZDirection;
    std::array<glm::vec4, utils::Frustum::numPoints()> frustumPoints;
    std::array<std::array<glm::vec4, 2u>, utils::Frustum::numEdges()> frustumEdges;
    std::array<glm::vec4, utils::Frustum::numPlanes()> frustumPlanes;

    static CameraDescription make(const utils::Frustum&);
};

struct ClusterDescription
{
    glm::vec4 boundingBoxMinPointAndFirstLightNodeID;
    glm::vec4 boundingBoxMaxPoint;

    static ClusterDescription make(const utils::BoundingBox&);
};

struct MeshDescription
{
    glm::vec4 boundingBoxMinPointAndNumVertexData;
    glm::vec4 boundingBoxMaxPointAndNumElements;
    uint32_t vertexDataOffset;
    uint32_t elementsOffset; // draw arrays is used if 0xFFFFFFFFu
    uint32_t flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
    uint32_t padding[1u];

    static MeshDescription make(
        const utils::BoundingBox&,
        uint32_t numVertexData,
        uint32_t numElements,
        uint32_t vertexDataOffset,
        uint32_t elementsOffset,
        uint32_t numPositionComponents,
        uint32_t numNormalComponents,
        uint32_t numTexCoordsComponents,
        uint32_t numBones,
        bool hasTangent,
        uint32_t numColorComponents);
    static uint32_t numVertexData(const MeshDescription&);
    static uint32_t numElements(const MeshDescription&);
};

struct MaterialMapDescription
{
    graphics::TextureHandle handle;

    static MaterialMapDescription make(graphics::TextureHandle);
};

struct MaterialDescription
{
    glm::vec4 baseColor;
    glm::vec4 emission;
    uint32_t baseColorTextureID;
    uint32_t emissionTextureID;
    uint32_t occlusionTextureID;
    uint32_t roughnessTextureID;
    uint32_t metalnessTextureID;
    uint32_t normalTextureID;
    uint32_t flags0; // 0.. 7 - roughness, 8..15 - metalness, 16..23 - occlusionMapStrength, 24..31 - normalMapScale
    uint32_t flags1;
    //  0.. 1 - occlusion map swizzle
    //  2.. 3 - roughness map swizzle
    //  4.. 5 - metalness map swizzle
    //  6.. 6 - is lighted
    //  7.. 7 - is shadowed
    //  8.. 8 - is transparent
    //  9.. 9 - is double sided
    // 10..17 - alpha cutoff
    // 18..31 - free (14 bits)

    static MaterialDescription make(
        const glm::vec4& baseColor,
        const glm::vec4& emission,
        uint32_t baseColorTextureID,
        uint32_t emissionTextureID,
        uint32_t occlusionTextureID,
        uint32_t roughnessTextureID,
        uint32_t metalnessTextureID,
        uint32_t normalTextureID,
        float roughness,
        float metalness,
        float occlusionMapStrength,
        float normalMapScale,
        const glm::u32vec3& ORMSwizzleMask,
        bool isLighted,
        bool isShadowed,
        bool isTransparent,
        bool isDoubleSided,
        float alphaCutoff);
};

struct DrawableDescription
{
    uint32_t meshID;
    uint32_t materialID;

    static DrawableDescription make(uint32_t meshID, uint32_t materialID);
};

struct DrawDataBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct DrawDataDescription
{
    TransformDescription transform;
    uint32_t drawableID;
    uint32_t padding[3u];

    static DrawDataDescription makeEmpty();
    static DrawDataDescription make(const utils::Transform& transform, uint32_t drawableID);
};

struct BackgroundDescription
{
    glm::vec4 environmentColorAndBlurPower;
    uint32_t environmentMapID;

    static BackgroundDescription makeEmpty();
    static BackgroundDescription make(const glm::vec3& environmentColor, float blurPower, uint32_t environmentMapID);
};

struct LightBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct LightDescription
{
    TransformDescription transform;
    glm::vec4 flags0;
    glm::vec4 flags1;

    static LightDescription makeEmpty();
    static LightDescription makePoint(const utils::Transform&, const glm::vec3&, const glm::vec2&);
    static LightDescription makeSpot(const utils::Transform&, const glm::vec3&, const glm::vec2&, const glm::vec2&);
    static LightDescription makeDirectional(const utils::Transform&, const glm::vec3&);
    static LightDescription makeImageBased(const utils::Transform&, uint32_t, uint32_t, uint32_t, float);
};

}
}

#endif // CORE_DESCRIPTIONS_H
