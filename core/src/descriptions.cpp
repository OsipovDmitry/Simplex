#include <utils/idgenerator.h>

#include <core/lightnode.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

static const uint32_t PointLightTypeID = castFromLightType(LightType::Point);
static const uint32_t SpotLightTypeID = castFromLightType(LightType::Spot);
static const uint32_t DirectionalLightTypeID = castFromLightType(LightType::Directional);
static const uint32_t ImageBasedLightTypeID = castFromLightType(LightType::ImageBased);
static const uint32_t EmptyLightTypeID = std::numeric_limits<uint32_t>::max();


TransformDescription TransformDescription::makeEmpty()
{
    return TransformDescription::make(utils::Transform::makeIdentity());
}

TransformDescription TransformDescription::make(const utils::Transform& value)
{
    return {
        glm::vec4(value.rotation.x, value.rotation.y, value.rotation.z, value.rotation.w),
        glm::vec4(value.translation, value.scale) };
}

CameraDescription CameraDescription::make(const utils::Frustum& value)
{
    const auto& viewTransform = value.viewTransform();
    const auto viewTransformInverted = value.viewTransform().inverted();

    CameraDescription desc{};
    desc.viewTransform = TransformDescription::make(viewTransform);
    desc.viewTransformInverted = TransformDescription::make(viewTransformInverted);
    desc.viewPosition = glm::vec4(viewTransformInverted.transformPoint(glm::vec3(0.f)), 1.f);
    desc.viewXDirection = glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(1.f, 0.f, 0.f))), 0.f);
    desc.viewYDirection = glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(0.f, 1.f, 0.f))), 0.f);
    desc.viewZDirection = glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(0.f, 0.f, 1.f))), 0.f);
    desc.projectionMatrix = value.projectionMatrix();
    desc.projectionMatrixInverted = glm::inverse(desc.projectionMatrix);
    desc.viewProjectionMatrix = desc.projectionMatrix * viewTransform;
    desc.viewProjectionMatrixInverted = glm::inverse(desc.viewProjectionMatrix);
    for (size_t i = 0u; i < utils::Frustum::numPoints(); ++i)
        desc.frustumPoints[i] = glm::vec4(value.points().at(i), 1.f);
    for (size_t i = 0u; i < utils::Frustum::numEdges(); ++i)
    {
        const auto& edge = value.edge(i);
        desc.frustumEdges[i] = { glm::vec4(edge.startPoint(), 1.f), glm::vec4(edge.endPoint(), 1.f) };
    }
    for (size_t i = 0u; i < utils::Frustum::numPlanes(); ++i)
        desc.frustumPlanes[i] = value.plane(utils::Frustum::castToPlaneIndex(i));

    return desc;
}

ClusterDescription ClusterDescription::make(const utils::BoundingBox& bb)
{
    return {
        glm::vec4(bb.minPoint(), glm::uintBitsToFloat(utils::IDsGenerator::last())),
        glm::vec4(bb.maxPoint(), 0u),
    };
}

MeshDescription MeshDescription::make(
    const utils::BoundingBox& bb,
    uint32_t numVertexData,
    uint32_t numElements,
    uint32_t vertexDataOffset,
    uint32_t elementsOffset,
    uint32_t numPositionComponents,
    uint32_t numNormalComponents,
    uint32_t numTexCoordsComponents,
    uint32_t numBones,
    bool hasTangent,
    uint32_t numColorComponents)
{
    uint32_t flags = 0u;
    flags |= (numPositionComponents & 0x3u) << 0u;
    flags |= (numNormalComponents & 0x3u) << 2u;
    flags |= (numTexCoordsComponents & 0x3u) << 4u;
    flags |= (numBones & 0x7u) << 6u;
    flags |= ((hasTangent ? 1u : 0u) & 0x1u) << 9u;
    flags |= (numColorComponents & 0x7u) << 10u;

    return {
        glm::vec4(bb.minPoint(), glm::uintBitsToFloat(numVertexData)),
        glm::vec4(bb.maxPoint(), glm::uintBitsToFloat(numElements)),
        vertexDataOffset,
        elementsOffset,
        flags };
}

uint32_t MeshDescription::numVertexData(const MeshDescription& desc)
{
    return glm::floatBitsToUint(desc.boundingBoxMinPointAndNumVertexData.w);
}

uint32_t MeshDescription::numElements(const MeshDescription& desc)
{
    return glm::floatBitsToUint(desc.boundingBoxMaxPointAndNumElements.w);
}

MaterialMapDescription MaterialMapDescription::make(graphics::TextureHandle textureHandle)
{
    return { textureHandle };
}

MaterialDescription MaterialDescription::make(
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
    float alphaCutoff)
{
    uint32_t flags0 = 0u;
    flags0 |= (static_cast<uint32_t>(glm::clamp(roughness, 0.f, 1.f) * 255.f) & 0xFF) << 0u;
    flags0 |= (static_cast<uint32_t>(glm::clamp(metalness, 0.f, 1.f) * 255.f) & 0xFF) << 8u;
    flags0 |= (static_cast<uint32_t>(glm::clamp(occlusionMapStrength, 0.f, 1.f) * 255.f) & 0xFF) << 16u;
    flags0 |= (static_cast<uint32_t>(glm::clamp(normalMapScale, 0.f, 1.f) * 255.f) & 0xFF) << 24u;

    uint32_t flags1 = 0u;
    flags1 |= (ORMSwizzleMask.r & 0x3u) << 0u;
    flags1 |= (ORMSwizzleMask.g & 0x3u) << 2u;
    flags1 |= (ORMSwizzleMask.b & 0x3u) << 4u;
    flags1 |= ((isLighted ? 1u : 0u) & 0x1u) << 6u;
    flags1 |= ((isShadowed ? 1u : 0u) & 0x1u) << 7u;
    flags1 |= ((isTransparent ? 1u : 0u) & 0x1u) << 8u;
    flags1 |= ((isDoubleSided ? 1u : 0u) & 0x1u) << 9u;
    flags1 |= (static_cast<uint32_t>(glm::clamp(alphaCutoff, 0.f, 1.f) * 255.f) & 0xFF) << 10u;
    
    return {
        baseColor,
        emission,
        baseColorTextureID,
        emissionTextureID,
        occlusionTextureID,
        roughnessTextureID,
        metalnessTextureID,
        normalTextureID,
        flags0,
        flags1 };
}

DrawableDescription DrawableDescription::make(uint32_t meshID, uint32_t materialID)
{
    return { meshID, materialID };
}

DrawDataDescription DrawDataDescription::makeEmpty()
{
    return { TransformDescription::makeEmpty(), utils::IDsGenerator::last() };
}

DrawDataDescription DrawDataDescription::make(const utils::Transform& transform, uint32_t drawableID)
{
    return { TransformDescription::make(transform), drawableID };
}

BackgroundDescription BackgroundDescription::makeEmpty()
{
    return BackgroundDescription::make(glm::vec3(1.f), 0.f, utils::IDsGenerator::last());
}

BackgroundDescription BackgroundDescription::make(const glm::vec3& environmentColor, float blurPower, uint32_t environmentMapID)
{
    return { glm::vec4(environmentColor, blurPower), environmentMapID };
}

LightDescription LightDescription::makeEmpty()
{
    return {
        TransformDescription::make(utils::Transform::makeIdentity()),
        glm::vec4(glm::vec3(0.f), glm::uintBitsToFloat(EmptyLightTypeID)),
        glm::vec4(0.f) };
}

LightDescription LightDescription::makePoint(
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses)
{
    return {
        TransformDescription::make(transform),
        glm::vec4(color, glm::uintBitsToFloat(PointLightTypeID)),
        glm::vec4(radiuses, 0.f, 0.f) };
}

LightDescription LightDescription::makeSpot(
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses,
    const glm::vec2& halfAngles)
{
    return {
        TransformDescription::make(transform),
        glm::vec4(color, glm::uintBitsToFloat(SpotLightTypeID)),
        glm::vec4(radiuses, glm::cos(halfAngles)) };
}

LightDescription LightDescription::makeDirectional(
    const utils::Transform& transform,
    const glm::vec3& color)
{
    return {
        TransformDescription::make(transform),
        glm::vec4(color, glm::uintBitsToFloat(DirectionalLightTypeID)),
        glm::vec4(0.f) };
}

LightDescription LightDescription::makeImageBased(
    const utils::Transform& transform,
    uint32_t BRDFLutMapID,
    uint32_t diffuseMapID,
    uint32_t specularMapID,
    float contribution)
{
    return {
        TransformDescription::make(transform),
        glm::vec4(glm::vec3(), glm::uintBitsToFloat(ImageBasedLightTypeID)),
        glm::vec4(
            glm::uintBitsToFloat(BRDFLutMapID),
            glm::uintBitsToFloat(diffuseMapID),
            glm::uintBitsToFloat(specularMapID),
            contribution) };
}

}
}
