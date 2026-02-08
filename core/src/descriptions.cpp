#include <utils/boundingbox.h>
#include <utils/clipspace.h>
#include <utils/idgenerator.h>
#include <utils/transform.h>

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

QuatDescription QuatDescription::make(const glm::quat& value)
{
    return { glm::vec4(value.x, value.y, value.z, value.w) };
}

TransformDescription TransformDescription::makeEmpty()
{
    return TransformDescription::make(utils::Transform::makeIdentity());
}

TransformDescription TransformDescription::make(const utils::Transform& value)
{
    return {
        QuatDescription::make(value.rotation),
        glm::vec4(value.translation, value.scale) };
}

ClipSpaceDescription ClipSpaceDescription::make(const utils::ClipSpace& clipSpace)
{
    return {
        clipSpace.params(),
        utils::castFromClipSpaceType(clipSpace.type()) };
}

CameraDescription CameraDescription::make(
    const glm::uvec3& clusterMaxSize,
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits)
{
    const auto viewTransformInverted = viewTransform.inverted();
    const auto projectionMatrix = clipSpace.projectionMatrix(cullPlaneLimits);
    const auto viewProjectionMatrix = projectionMatrix * viewTransform;

    return {
        glm::uvec4(clusterMaxSize, 0u),
        TransformDescription::make(viewTransform),
        ClipSpaceDescription::make(clipSpace),
        glm::vec2(cullPlaneLimits.nearValue(), cullPlaneLimits.farValue()),
        glm::uvec2(glm::floatBitsToUint(FLT_MAX), glm::floatBitsToUint(0.f)),
        TransformDescription::make(viewTransformInverted),
        glm::vec4(viewTransformInverted.transformPoint(glm::vec3(0.f)), 1.f),
        glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(1.f, 0.f, 0.f))), 0.f),
        glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(0.f, 1.f, 0.f))), 0.f),
        glm::vec4(glm::normalize(viewTransformInverted.transformVector(glm::vec3(0.f, 0.f, 1.f))), 0.f),
        projectionMatrix,
        glm::inverse(projectionMatrix),
        viewProjectionMatrix,
        glm::inverse(viewProjectionMatrix) };
}

SceneInfoDescription SceneInfoDescription::make(
    uint32_t time,
    uint32_t drawDataCount,
    uint32_t skeletalAnimatedDataCount,
    uint32_t lightsCount,
    uint32_t lightNodesMaxCount)
{
    return {
        time,
        drawDataCount,
        skeletalAnimatedDataCount,
        lightsCount,
        0u,
        0u,
        lightNodesMaxCount,
        0u,
        0xFFFFFFFFu};
}

GBufferDescription GBufferDescription::make(
    graphics::TextureHandle colorTextureHandle,
    graphics::TextureHandle depthTextureHandle,
    graphics::ImageHandle OITIndicesImageHandle,
    graphics::TextureHandle finalTextureHandle,
    const glm::uvec2& size,
    uint32_t OITNodesMaxCount)
{
    return {
        colorTextureHandle,
        depthTextureHandle,
        OITIndicesImageHandle,
        finalTextureHandle,
        size,
        OITNodesMaxCount,
        0u };
}

MeshDescription MeshDescription::makeEmpty()
{
    return make(
        utils::BoundingBox::empty(),
        0u,
        0u,
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        0u,
        0u,
        0u,
        0u,
        false,
        0u);
}

MeshDescription MeshDescription::make(
    const utils::BoundingBox& bb,
    uint32_t verticesDataSize,
    uint32_t elementsDataSize,
    uint32_t verticesDataOffset,
    uint32_t elementsDataOffset,
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
        glm::vec4(bb.minPoint(), glm::uintBitsToFloat(verticesDataSize)),
        glm::vec4(bb.maxPoint(), glm::uintBitsToFloat(elementsDataSize)),
        verticesDataOffset,
        elementsDataOffset,
        flags };
}

uint32_t MeshDescription::verticesDataSize(const MeshDescription& desc)
{
    return glm::floatBitsToUint(desc.boundingBoxMinPointAndVerticesDataSize.w);
}

uint32_t MeshDescription::elementsDataSize(const MeshDescription& desc)
{
    return glm::floatBitsToUint(desc.boundingBoxMaxPointAndElementsDataSize.w);
}

MaterialMapDescription MaterialMapDescription::makeEmpty()
{
    return make(utils::IDsGeneratorT<graphics::TextureHandle>::last());
}

MaterialMapDescription MaterialMapDescription::make(graphics::TextureHandle textureHandle)
{
    return { textureHandle };
}

MaterialDescription MaterialDescription::makeEmpty()
{
    return make(
        glm::vec4(),
        glm::vec3(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        0.f,
        0.f,
        0.f,
        0.f,
        glm::u32vec3(),
        false,
        false,
        false,
        false,
        false,
        0.f);
}

MaterialDescription MaterialDescription::make(
    const glm::vec4& baseColor,
    const glm::vec3& emission,
    uint32_t baseColorMapID,
    uint32_t opacityMapID,
    uint32_t emissionMapID,
    uint32_t occlusionMapID,
    uint32_t roughnessMapID,
    uint32_t metalnessMapID,
    uint32_t normalMapID,
    float roughness,
    float metalness,
    float occlusionMapStrength,
    float normalMapScale,
    const glm::u32vec3& ORMSwizzleMask,
    bool isLighted,
    bool isShadowed,
    bool isShadowCasted,
    bool isDoubleSided,
    bool isTransparent,
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
    flags1 |= ((isShadowCasted ? 1u : 0u) & 0x1u) << 8u;
    flags1 |= ((isDoubleSided ? 1u : 0u) & 0x1u) << 9u;
    flags1 |= ((isTransparent ? 1u : 0u) & 0x1u) << 10u;
    flags1 |= (static_cast<uint32_t>(glm::clamp(alphaCutoff, 0.f, 1.f) * 255.f) & 0xFF) << 11u;
    
    return {
        baseColor,
        glm::vec4(emission, 1.f),
        baseColorMapID,
        opacityMapID,
        emissionMapID,
        occlusionMapID,
        roughnessMapID,
        metalnessMapID,
        normalMapID,
        flags0,
        flags1 };
}

DrawableDescription DrawableDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), utils::IDsGenerator::last());
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
    return BackgroundDescription::make(glm::quat(), glm::vec3(1.f), 0.f, utils::IDsGenerator::last());
}

BackgroundDescription BackgroundDescription::make(
    const glm::quat& rotation,
    const glm::vec3& environmentColor,
    float blurPower,
    uint32_t environmentMapID)
{
    return { QuatDescription::make(rotation), glm::vec4(environmentColor, blurPower), environmentMapID };
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
        glm::vec4(radiuses, halfAngles) };
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

SkeletonDescription SkeletonDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), 0u);
}

SkeletonDescription SkeletonDescription::make(uint32_t dataOffset, uint32_t dataSize)
{
    return { dataOffset, dataSize };
}

SkeletalAnimatedDataDescription SkeletalAnimatedDataDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), utils::IDsGenerator::last(), utils::IDsGenerator::last(), 0u);
}

SkeletalAnimatedDataDescription SkeletalAnimatedDataDescription::make(
    uint32_t skeletonID,
    uint32_t currentAnimationID,
    uint32_t bonesTransfromsDataOffset,
    uint32_t bonesTransfromsDataSize)
{
    return { skeletonID, currentAnimationID, bonesTransfromsDataOffset, bonesTransfromsDataSize };
}

}
}
