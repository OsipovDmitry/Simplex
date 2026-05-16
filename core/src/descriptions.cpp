#include "descriptions.h"

#include <utils/boundingbox.h>
#include <utils/clipspace.h>
#include <utils/idgenerator.h>
#include <utils/orientedboundingbox.h>
#include <utils/range.h>
#include <utils/transform.h>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

static const uint32_t PointLightTypeID = castFromLightType(LightType::Point);
static const uint32_t SpotLightTypeID = castFromLightType(LightType::Spot);
static const uint32_t DirectionalLightTypeID = castFromLightType(LightType::Directional);
static const uint32_t ImageBasedLightTypeID = castFromLightType(LightType::ImageBased);
static const uint32_t AmbientLightTypeID = castFromLightType(LightType::Ambient);
static const uint32_t UndefinedLightTypeID = std::numeric_limits<uint32_t>::max();

QuatDescription QuatDescription::make(const glm::quat& value)
{
    return {glm::vec4(value.x, value.y, value.z, value.w)};
}

RangeDescription RangeDescription::make(const utils::Range& value)
{
    return {static_cast<glm::vec2>(value)};
}

PlaneDescription PlaneDescription::make(const utils::Plane& value)
{
    return {static_cast<glm::vec4>(value)};
}

BoundingBoxDescription BoundingBoxDescription::make(const utils::BoundingBox& bb)
{
    return {glm::vec4(bb.minPoint(), 0.0f), glm::vec4(bb.maxPoint(), 0.0f)};
}

OrientedBoundingBoxDescription OrientedBoundingBoxDescription::make(const utils::OrientedBoundingBox& obb)
{
    return {QuatDescription::make(obb.rotation()), glm::vec4(obb.translation(), 0.0f), glm::vec4(obb.halfSizes(), 0.0f)};
}

TransformDescription TransformDescription::makeEmpty()
{
    return TransformDescription::make(utils::Transform::makeIdentity());
}

TransformDescription TransformDescription::make(const utils::Transform& value)
{
    return {QuatDescription::make(value.rotation), glm::vec4(value.translation, value.scale)};
}

ClipSpaceDescription ClipSpaceDescription::make(const utils::ClipSpace& clipSpace)
{
    return {clipSpace.params(), utils::castFromClipSpaceType(clipSpace.type())};
}

RenderInfoDescription RenderInfoDescription::make(
    uint32_t time,
    float dielectricSpecular,
    const utils::OrientedBoundingBox& globalBoundingBox,
    uint32_t drawDataCount,
    uint32_t skeletalAnimatedDataCount,
    uint32_t shadowsCount,
    uint32_t lightsCount,
    graphics::TextureHandle shadowVarianceBluredTextureHandle,
    graphics::TextureHandle shadowColorBluredTextureHandle,
    const std::vector<float>& shadowBlurKernel,
    float shadowLightBleedingAmount,
    uint32_t shadowAtlasSize,
    const glm::uvec3& clusterSize,
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits)
{
    RenderInfoDescription result{};

    // global
    result.time = time;
    result.dielectricSpecular = dielectricSpecular;

    // scene
    result.globalBoundingBox = OrientedBoundingBoxDescription::make(globalBoundingBox);
    result.drawDataCount = drawDataCount;
    result.skeletalAnimatedDataCount = skeletalAnimatedDataCount;
    result.shadowsCount = shadowsCount;
    result.lightsCount = lightsCount;

    // shadow
    result.shadowVarianceBluredTextureHandle = shadowVarianceBluredTextureHandle;
    result.shadowColorBluredTextureHandle = shadowColorBluredTextureHandle;
    for (size_t i = 0u; i < shadowBlurKernel.size(); ++i)
        result.shadowBlurKernel[i] = shadowBlurKernel[i];
    result.shadowBlurRadius = static_cast<uint32_t>(shadowBlurKernel.size());
    result.shadowLightBleedingAmount = shadowLightBleedingAmount;
    result.shadowAtlasSize = shadowAtlasSize;

    // camera
    result.clusterSize = glm::uvec4(clusterSize, 0u);
    result.viewTransform = TransformDescription::make(viewTransform);
    result.clipSpace = ClipSpaceDescription::make(clipSpace);
    result.cullPlaneLimits = RangeDescription::make(cullPlaneLimits);

    return result;
}

GBufferDescription GBufferDescription::make(
    graphics::TextureHandle colorTextureHandle,
    graphics::TextureHandle depthTextureHandle,
    graphics::ImageHandle OITIndicesImageHandle,
    graphics::TextureHandle finalTextureHandle,
    const glm::uvec2& size,
    uint32_t OITNodesMaxCount)
{
    return {colorTextureHandle, depthTextureHandle, OITIndicesImageHandle, finalTextureHandle, size, OITNodesMaxCount, 0u};
}

MeshDescription MeshDescription::makeEmpty()
{
    return make(
        utils::BoundingBox::empty(), 0u, 0u, utils::IDsGenerator::last(), utils::IDsGenerator::last(), 0u, 0u, 0u, 0u, false, 0u);
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

    return {BoundingBoxDescription::make(bb), verticesDataSize, elementsDataSize, verticesDataOffset, elementsDataOffset, flags};
}

MapDescription MapDescription::makeEmpty()
{
    return make(utils::IDsGeneratorT<graphics::TextureHandle>::last());
}

MapDescription MapDescription::make(graphics::TextureHandle textureHandle)
{
    return {textureHandle};
}

MaterialDescription MaterialDescription::makeEmpty()
{
    return make(
        glm::vec4(), glm::vec3(), utils::IDsGenerator::last(), utils::IDsGenerator::last(), utils::IDsGenerator::last(),
        utils::IDsGenerator::last(), utils::IDsGenerator::last(), utils::IDsGenerator::last(), utils::IDsGenerator::last(), 0.f,
        0.f, 0.f, 0.f, glm::u32vec3(), false, false, false, false, false, 0.f);
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

    return {baseColor,      glm::vec4(emission, 1.f), baseColorMapID, opacityMapID, emissionMapID, occlusionMapID,
            roughnessMapID, metalnessMapID,           normalMapID,    flags0,       flags1};
}

DrawableDescription DrawableDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), utils::IDsGenerator::last());
}

DrawableDescription DrawableDescription::make(uint32_t meshID, uint32_t materialID)
{
    return {meshID, materialID};
}

DrawDataDescription DrawDataDescription::makeEmpty()
{
    return {TransformDescription::makeEmpty(), utils::IDsGenerator::last(), utils::IDsGenerator::last()};
}

DrawDataDescription DrawDataDescription::make(
    const utils::Transform& transform,
    uint32_t drawableID,
    uint32_t skeletalAnimatedDataID)
{
    return {TransformDescription::make(transform), drawableID, skeletalAnimatedDataID};
}

BackgroundDescription BackgroundDescription::makeEmpty()
{
    return BackgroundDescription::make(glm::quat(1.f, 0.f, 0.f, 0.f), glm::vec3(1.f), 0.f, utils::IDsGenerator::last());
}

ShadowTransformsDataDescription ShadowTransformsDataDescription::make(
    const utils::Transform& viewTransform,
    const utils::Range& ZRange,
    const glm::mat4x4& projectionMatrix,
    const glm::uvec3& mapCoords,
    uint32_t packerLayerID)
{
    return {
        TransformDescription::make(viewTransform), RangeDescription::make(ZRange), projectionMatrix,
        glm::uvec4(mapCoords, packerLayerID)};
}

ShadowTransformsDataDescription ShadowTransformsDataDescription::make(const glm::uvec3& mapCoords, uint32_t packerLayerID)
{
    return make(utils::Transform::makeIdentity(), utils::Range(), glm::mat4x4(1.f), mapCoords, packerLayerID);
}

glm::uvec3 ShadowTransformsDataDescription::mapCoords(const ShadowTransformsDataDescription& desc)
{
    return glm::uvec3(desc.mapCoordsAndPackerItemID);
}

uint32_t ShadowTransformsDataDescription::packerItemID(const ShadowTransformsDataDescription& desc)
{
    return desc.mapCoordsAndPackerItemID[3u];
}

BackgroundDescription BackgroundDescription::make(
    const glm::quat& rotation,
    const glm::vec3& environmentColor,
    float blurPower,
    uint32_t environmentMapID)
{
    return {QuatDescription::make(rotation), glm::vec4(environmentColor, blurPower), environmentMapID};
}

ShadowDescription ShadowDescription::makeEmpty()
{
    return make(0u, utils::Range(), 0u, utils::IDsGenerator::last());
}

ShadowDescription ShadowDescription::make(
    uint32_t mapSize,
    const utils::Range& cullPlaneLimits,
    uint32_t layersCount,
    uint32_t transformsDataOffset)
{
    uint32_t flags = 0u;
    flags |= (layersCount & 0x7u) << 0u;

    return {RangeDescription::make(cullPlaneLimits), mapSize, transformsDataOffset, flags};
}

uint32_t ShadowDescription::layersCount(const ShadowDescription& desc)
{
    return glm::bitfieldExtract(desc.flags, 0, 3);
}

LightDescription LightDescription::makeEmpty()
{
    return {
        TransformDescription::make(utils::Transform::makeIdentity()),
        glm::vec4(glm::vec3(0.f), glm::uintBitsToFloat(UndefinedLightTypeID)), glm::vec4(0.f), utils::IDsGenerator::last(), 0u};
}

LightDescription LightDescription::makePoint(
    const utils::Transform& transform,
    bool isEnabled,
    const glm::vec3& color,
    const glm::vec2& radiuses,
    uint32_t shadowID)
{
    uint32_t flags = 0u;
    flags |= (isEnabled & 0x1u) << 0u;

    return {
        TransformDescription::make(transform), glm::vec4(color, glm::uintBitsToFloat(PointLightTypeID)),
        glm::vec4(radiuses, 0.f, 0.f), shadowID, flags};
}

LightDescription LightDescription::makeSpot(
    const utils::Transform& transform,
    bool isEnabled,
    const glm::vec3& color,
    const glm::vec2& radiuses,
    const glm::vec2& halfAngles,
    uint32_t shadowID)
{
    uint32_t flags = 0u;
    flags |= (isEnabled & 0x1u) << 0u;

    return {
        TransformDescription::make(transform), glm::vec4(color, glm::uintBitsToFloat(SpotLightTypeID)),
        glm::vec4(radiuses, halfAngles), shadowID, flags};
}

LightDescription LightDescription::makeDirectional(
    const utils::Transform& transform,
    bool isEnabled,
    const glm::vec3& color,
    uint32_t shadowID)
{
    uint32_t flags = 0u;
    flags |= (isEnabled & 0x1u) << 0u;

    return {
        TransformDescription::make(transform), glm::vec4(color, glm::uintBitsToFloat(DirectionalLightTypeID)), glm::vec4(0.f),
        shadowID, flags};
}

LightDescription LightDescription::makeImageBased(
    const utils::Transform& transform,
    bool isEnabled,
    uint32_t BRDFLutMapID,
    uint32_t diffuseMapID,
    uint32_t specularMapID,
    float contribution)
{
    uint32_t flags = 0u;
    flags |= (isEnabled & 0x1u) << 0u;

    return {
        TransformDescription::make(transform), glm::vec4(glm::vec3(0.f), glm::uintBitsToFloat(ImageBasedLightTypeID)),
        glm::vec4(
            glm::uintBitsToFloat(BRDFLutMapID), glm::uintBitsToFloat(diffuseMapID), glm::uintBitsToFloat(specularMapID),
            contribution),
        utils::IDsGenerator::last(), flags};
}

LightDescription LightDescription::makeAmbient(bool isEnabled, const glm::vec3& color)
{
    uint32_t flags = 0u;
    flags |= (isEnabled & 0x1u) << 0u;

    return {
        TransformDescription::makeEmpty(), glm::vec4(color, glm::uintBitsToFloat(AmbientLightTypeID)), glm::vec4(0.f),
        utils::IDsGenerator::last(), flags};
}

SkeletonDescription SkeletonDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), 0u);
}

SkeletonDescription SkeletonDescription::make(uint32_t dataOffset, uint32_t dataSize)
{
    return {dataOffset, dataSize};
}

ShadowMapsDescription ShadowMapsDescription::makeEmpty()
{
    return ShadowMapsDescription::make(
        utils::IDsGeneratorT<graphics::TextureHandle>::last(), utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        utils::IDsGeneratorT<graphics::TextureHandle>::last());
}

ShadowMapsDescription ShadowMapsDescription::make(
    graphics::TextureHandle shadowDepthTextureHandle,
    graphics::TextureHandle shadowVarianceTextureHandle,
    graphics::TextureHandle shadowColorTextureHandle)
{
    return {shadowDepthTextureHandle, shadowVarianceTextureHandle, shadowColorTextureHandle};
}

SkeletalAnimatedDataDescription SkeletalAnimatedDataDescription::makeEmpty()
{
    return make(utils::IDsGenerator::last(), utils::IDsGenerator::last(), utils::IDsGenerator::last(), 0u, 0u);
}

SkeletalAnimatedDataDescription SkeletalAnimatedDataDescription::make(
    uint32_t skeletonID,
    uint32_t currentAnimationID,
    uint32_t bonesTransformsDataOffset,
    uint32_t bonesTransformsDataSize,
    uint32_t lastUpdateTime)
{
    return {skeletonID, currentAnimationID, bonesTransformsDataOffset, bonesTransformsDataSize, lastUpdateTime};
}

} // namespace core
} // namespace simplex
