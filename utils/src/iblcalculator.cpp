#include <utils/glm/gtc/constants.hpp>
#include <utils/glm/gtx/polar_coordinates.hpp>
#include <utils/glm/gtx/texture.hpp>

#include <utils/logger.h>
#include <utils/image.h>
#include <utils/iblcalculator.h>

namespace simplex
{
namespace utils
{

uint32_t IBLCalculator::s_BRDFLutSize = 256u;
uint32_t IBLCalculator::s_BRDFLutSamplesCount = 1024u;

uint32_t IBLCalculator::s_irradianceSize = 64u;
float IBLCalculator::s_irradianceSapmlesDeltaAngle = .025f;

uint32_t IBLCalculator::s_radianceSize = 256u;
uint32_t IBLCalculator::s_radianceSamplesCount = 1024u;

inline bool checkImage(const std::shared_ptr<Image> &image)
{
    if (!image)
    {
        LOG_ERROR << "Image can't be nullptr";
        return false;
    }

    if (image->width() * image->height() == 0u)
    {
        LOG_ERROR << "Width and height of image can't be 0";
        return false;
    }

    if (image->width() != image->height() * 2u)
    {
        LOG_ERROR << "Width of image must be 2 * height of source image";
        return false;
    }

    if (image->numComponents() != 3u)
    {
        LOG_ERROR << "Components count of image must be 3";
        return false;
    }

    if (image->type() == PixelComponentType::Undefined)
    {
        LOG_ERROR << "Undefined pixel component type of image";
        return false;
    }

    if (!image->data())
    {
        LOG_ERROR << "Data of image can't be nullptr";
        return false;
    }

    return true;
}

inline glm::vec3 readPixel(const void *data,
                           uint32_t width,
                           size_t pixelSize,
                           uint32_t x,
                           uint32_t y,
                           PixelComponentType type)
{

    const auto *ptr = static_cast<const uint8_t*>(data) + pixelSize * (width * y + x);

    switch (type)
    {
    case PixelComponentType::Single:
        return *reinterpret_cast<const glm::vec3*>(ptr);
    case PixelComponentType::Uint8:
        return static_cast<const glm::vec3>(*reinterpret_cast<const glm::u8vec3*>(ptr)) / static_cast<float>(std::numeric_limits<uint8_t>::max());
    case PixelComponentType::Uint16:
        return static_cast<const glm::vec3>(*reinterpret_cast<const glm::u16vec3*>(ptr)) / static_cast<float>(std::numeric_limits<uint16_t>::max());
    default:
        return glm::vec3(0.f);
    }
}

inline void writePixel(void *data,
                       uint32_t width,
                       size_t pixelSize,
                       uint32_t x,
                       uint32_t y,
                       PixelComponentType type,
                       const glm::vec3 &value)
{
    auto *ptr = static_cast<uint8_t*>(data) + pixelSize * (width * y + x);

    switch (type)
    {
    case PixelComponentType::Single:
        *reinterpret_cast<glm::vec3*>(ptr) = value;
        break;
    case PixelComponentType::Uint8:
        *reinterpret_cast<glm::u8vec3*>(ptr) = static_cast<const glm::u8vec3>(value / static_cast<float>(std::numeric_limits<uint8_t>::max()));
        break;
    case PixelComponentType::Uint16:
        *reinterpret_cast<glm::u16vec3*>(ptr) = static_cast<const glm::u16vec3>(value / static_cast<float>(std::numeric_limits<uint8_t>::max()));
        break;
    default:
        break;
    }
}

inline glm::vec2 hammersley(uint32_t i, uint32_t N)
{
    return glm::vec2(static_cast<float>(i)/static_cast<float>(N), glm::bitfieldReverse(i) * 2.3283064365386963e-10);
}

inline glm::vec3 importanceSampleGGX(const glm::vec2 &Xi, const glm::vec3 &N, float roughness)
{
    const float a = roughness * roughness;

    const float phi = glm::two_pi<float>() * Xi.x;
    const float cosTheta = glm::sqrt((1.f - Xi.y) / (1.f + (a * a - 1.f) * Xi.y));
    const float sinTheta = glm::sqrt(1.f - cosTheta * cosTheta);

    const glm::vec3 H(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);

    const glm::vec3 up = glm::abs(N.z) < .999f ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(1.f, 0.f, 0.f);
    const glm::vec3 tangent = glm::normalize(glm::cross(up, N));
    const glm::vec3 bitangent = glm::cross(N, tangent);

    return glm::normalize(tangent * H.x + bitangent * H.y + N * H.z);
}

inline float geometrySchlickGGX(float NdotV, float roughness)
{
    const float k = (roughness * roughness) / 2.f;

    const float nom = NdotV;
    const float denom = NdotV * (1.f - k) + k;

    return nom / denom;
}

inline float geometrySmith(float NdotV, float NdotL, float roughness)
{
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

inline float distributionGGX(float NdotH, float roughness)
{
    const auto a = roughness * roughness;
    const auto a2 = a * a;
    const auto NdotH2 = NdotH * NdotH;

    const auto nom = a2;
    auto denom = (NdotH2 * (a2 - 1.f) + 1.f);
    denom = glm::pi<float>() * denom * denom;

    return nom / denom;
}

inline glm::vec2 calculateBRDFLutOnePixelImpl(float roughness, float NdotV, uint32_t samplesCount)
{
    static const glm::vec3 N(0.f, 0.f, 1.f);

    const glm::vec3 V(glm::sqrt(1.f - NdotV * NdotV), 0.f, NdotV);
    glm::vec2 AB(0.f);

    for (uint32_t i = 0; i < samplesCount; ++i)
    {
        const auto Xi = hammersley(i, samplesCount);
        const auto H = importanceSampleGGX(Xi, N, roughness);
        const auto L = normalize(2.f * dot(V, H) * H - V);

        const float NdotL = glm::max(L.z, 0.f);
        const float NdotH = glm::max(H.z, .001f);
        const float VdotH = glm::max(glm::dot(V, H), 0.f);

        if (NdotL > 0.f)
        {
            const float G = geometrySmith(NdotV, NdotL, roughness);
            const float G_Vis = (G * VdotH) / (NdotH * NdotV);
            const float Fc = glm::pow(1.f - VdotH, 5.f);

            AB += glm::vec2((1.f - Fc) * G_Vis, Fc * G_Vis);
        }
    }

    return AB / static_cast<float>(samplesCount);
}

inline glm::vec3 calculateIrradianceOnePixelImpl(const std::shared_ptr<Image> &src,
                                                 size_t pixelSize,
                                                 const glm::vec3 &normal,
                                                 const glm::vec3 &tangent,
                                                 const glm::vec3 &bitangent,
                                                 float sampleDeltaAngle)
{
    glm::vec3 irradiance(0.f);
    uint32_t samplesCount = 0u;

    float phi = 0.f;
    while (phi < glm::two_pi<float>())
    {
        const auto cosPhi = glm::cos(phi);
        const auto sinPhi = glm::sin(phi);

        float theta = 0.f;
        while (theta < glm::half_pi<float>())
        {
            const auto cosTheta = glm::cos(theta);
            const auto sinTheta = glm::sin(theta);

            const glm::vec3 tangentSample(sinTheta * cosPhi,  sinTheta * sinPhi, cosTheta);
            const auto sampleVec = tangent * tangentSample.x + bitangent * tangentSample.y + normal * tangentSample.z;

            auto polar = glm::polar(sampleVec);
            const float latitude = glm::clamp(.5f + polar.x / glm::pi<float>(), 0.f, 1.f);
            const float longitude = glm::clamp(.5f - polar.y / glm::two_pi<float>(), 0.f, 1.f);

            const auto srcX = static_cast<uint32_t>(longitude * static_cast<float>(src->width() - 1u) + .5f);
            const auto srcY = static_cast<uint32_t>(latitude * static_cast<float>(src->height() - 1u) + .5f);

            irradiance += readPixel(src->data(),
                                    src->width(),
                                    pixelSize,
                                    srcX,
                                    srcY,
                                    src->type()) * cosTheta * sinTheta;
            ++samplesCount;
            theta += sampleDeltaAngle;
        }

        phi += sampleDeltaAngle;
    }

    irradiance *= glm::pi<float>() / static_cast<float>(samplesCount);
    return irradiance;
}

inline glm::vec3 calculateRadianceOnePixelImpl(const std::shared_ptr<Image> &src,
                                               size_t pixelSize,
                                               float roughness,
                                               const glm::vec3 &normal,
                                               uint32_t samplesCount)
{
    const auto R = normal;
    const auto V = R;

    glm::vec3 prefilteredColor(0.f);
    float totalWeight = 0.f;

    for(uint32_t i = 0u; i < samplesCount; ++i)
    {
        const auto Xi = hammersley(i, samplesCount);
        const auto H = importanceSampleGGX(Xi, normal, roughness);
        const auto L  = normalize(2.f * glm::dot(V, H) * H - V);

        const auto NdotL = glm::max(glm::dot(normal, L), 0.f);
        if (NdotL > 0.f)
        {
            const auto polar = glm::polar(L);
            const float latitude = glm::clamp(.5f + polar.x / glm::pi<float>(), 0.f, 1.f);
            const float longitude = glm::clamp(.5f - polar.y / glm::two_pi<float>(), 0.f, 1.f);

            const auto srcX = static_cast<uint32_t>(longitude * static_cast<float>(src->width() - 1u) + .5f);
            const auto srcY = static_cast<uint32_t>(latitude * static_cast<float>(src->height() - 1u) + .5f);

            prefilteredColor += readPixel(src->data(),
                                          src->width(),
                                          pixelSize,
                                          srcX,
                                          srcY,
                                          src->type()) * NdotL;
            totalWeight += NdotL;
        }
    }

    return prefilteredColor / totalWeight;
}

glm::vec2 IBLCalculator::calculateBRDFLutOnePixel(uint32_t x, uint32_t y)
{
    const auto roughness = 1.f - static_cast<float>(y) / static_cast<float>(s_BRDFLutSize - 1u);
    const auto NdotV = static_cast<float>(x) / static_cast<float>(s_BRDFLutSize - 1u);
    return calculateBRDFLutOnePixelImpl(roughness, NdotV, s_BRDFLutSamplesCount);
}

std::shared_ptr<Image> IBLCalculator::calclulateBRDFLut()
{
    static std::shared_ptr<Image> brdfLut;
    if (!brdfLut)
    {
        brdfLut = utils::Image::loadFromData(s_BRDFLutSize, s_BRDFLutSize, 3u, PixelComponentType::Uint8, nullptr);
        auto *brdfLutData = reinterpret_cast<glm::u8vec3*>(brdfLut->data());

        for (uint32_t y = 0u; y < brdfLut->height(); ++y)
        {
            const auto roughness = 1.f - static_cast<float>(y) / static_cast<float>(brdfLut->height() - 1u);
            for (uint32_t x = 0u; x < brdfLut->width(); ++x)
            {
                const auto NdotV = glm::max(static_cast<float>(x) / static_cast<float>(brdfLut->width() - 1u), .001f);
                const auto AB = calculateBRDFLutOnePixelImpl(roughness, NdotV, s_BRDFLutSamplesCount);

                brdfLutData[y * brdfLut->width() + x] = glm::u8vec3((.5f + std::numeric_limits<uint8_t>::max()) * glm::clamp(AB, 0.f, 1.f), 0u);
            }
        }
    }

    return brdfLut;
}

glm::vec3 IBLCalculator::calculateIrradianceOnePixel(const std::shared_ptr<Image> &src, float srcX, float srcY)
{
    if (!checkImage(src))
        return glm::vec3(0.f);

    const size_t pixelSize = src->numComponents() * sizeOfPixelComponentType(src->type());

    const auto polar = glm::vec2((srcX - .5f) * glm::pi<float>(), (.5f - srcY) * glm::two_pi<float>());
    const auto euclidean = glm::euclidean(polar);
    const auto tangent = glm::normalize(glm::cross(glm::abs(euclidean.z) < .999f ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(1.f, 0.f, 0.f),
                                                   euclidean));
    const auto bitangent = normalize(cross(euclidean, tangent));

    return calculateIrradianceOnePixelImpl(src, pixelSize, euclidean, tangent, bitangent, s_irradianceSapmlesDeltaAngle);
}

std::shared_ptr<Image> IBLCalculator::calculateIrradiance(const std::shared_ptr<Image> &src)
{
    if (!checkImage(src))
        return nullptr;

    std::shared_ptr<Image> result;

    const auto resultSize = glm::min(s_irradianceSize, src->height());
    result = Image::loadFromData(2u * resultSize, resultSize, src->numComponents(), src->type(), nullptr);

    const size_t pixelSize = src->numComponents() * sizeOfPixelComponentType(src->type());

    for (uint32_t dstY = 0u; dstY < result->height(); ++dstY)
    {
        glm::vec2 polar;
        polar.x = (static_cast<float>(dstY) / static_cast<float>(result->height() - 1u) - .5f) * glm::pi<float>();

        for (uint32_t dstX = 0u; dstX < result->width(); ++dstX)
        {
            polar.y = (.5f - static_cast<float>(dstX) / static_cast<float>(result->width() - 1u)) * glm::two_pi<float>();
            const auto euclidean = glm::euclidean(polar);
            const auto tangent = glm::normalize(glm::cross(abs(euclidean.z) < .999f ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(1.f, 0.f, 0.f),
                                                           euclidean));
            const auto bitangent = normalize(cross(euclidean, tangent));

            writePixel(result->data(),
                       result->width(),
                       pixelSize,
                       dstX,
                       dstY,
                       result->type(),
                       calculateIrradianceOnePixelImpl(src, pixelSize, euclidean, tangent, bitangent, s_irradianceSapmlesDeltaAngle));
        }
    }

    return result;
}

glm::vec3 IBLCalculator::calculateRadianceOnePixel(const std::shared_ptr<Image> &src, float roughness, float srcX, float srcY)
{
    if (!checkImage(src))
        return glm::vec3(0.f);

    const size_t pixelSize = src->numComponents() * sizeOfPixelComponentType(src->type());
    const auto polar = glm::vec2((srcX - .5f) * glm::pi<float>(), (.5f - srcY) * glm::two_pi<float>());
    const auto euclidean = glm::euclidean(polar);

    return calculateRadianceOnePixelImpl(src, pixelSize, roughness, euclidean, s_radianceSamplesCount);
}

std::vector<std::shared_ptr<Image>> IBLCalculator::calculateRadiance(const std::shared_ptr<Image> &src, uint32_t numLevels)
{
    if (!checkImage(src))
        return std::vector<std::shared_ptr<Image>>();

    const auto pixelSize = src->numComponents() * sizeOfPixelComponentType(src->type());

    const auto resultSize = glm::min(s_radianceSize, src->height());
    uint32_t resultWidth = 2u * resultSize;
    uint32_t resultHeight = resultSize;

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec1(resultHeight)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    std::vector<std::shared_ptr<Image>> result;
    result.resize(numLevels);

    for (uint32_t level = 0u; level < numLevels; ++level)
    {
        const float roughness = numLevels > 1u ?
                    static_cast<float>(level) / static_cast<float>(numLevels - 1u) :
                    0.f;

        auto dst = utils::Image::loadFromData(resultWidth, resultHeight, src->numComponents(), src->type(), nullptr);

        for (uint32_t dstY = 0u; dstY < resultHeight; ++dstY)
        {
            glm::vec2 polar;
            polar.x = (static_cast<float>(dstY) / static_cast<float>(dst->height() - 1u) - .5f) * glm::pi<float>();

            for (uint32_t dstX = 0u; dstX < resultWidth; ++dstX)
            {
                polar.y = (.5f - static_cast<float>(dstX) / static_cast<float>(dst->width() - 1u)) * glm::two_pi<float>();
                const auto euclidean = glm::euclidean(polar);

                writePixel(dst->data(),
                           resultWidth,
                           pixelSize,
                           dstX,
                           dstY,
                           dst->type(),
                           calculateRadianceOnePixelImpl(src, pixelSize, roughness, euclidean, s_radianceSamplesCount));
            }
        }

        result[level] = dst;
        resultWidth = glm::max(resultWidth / 2u, 1u);
        resultHeight = glm::max(resultHeight / 2u, 1u);
    }

    return result;
}

void IBLCalculator::setBRDFLutSize(uint32_t value)
{
    if (value == 0u)
    {
        LOG_ERROR << "Size of BRDF Lut image can't be 0";
        return;
    }

    s_BRDFLutSize = value;
}

void IBLCalculator::setBRDFLutSamplesCount(uint32_t value)
{
    if (value == 0u)
    {
        LOG_ERROR << "Samples count of BRDF Lut image can't be 0";
        return;
    }

    s_BRDFLutSamplesCount = value;
}

void IBLCalculator::setIrradianseSize(uint32_t value)
{
    if (value == 0u)
    {
        LOG_ERROR << "Size of irradiance image can't be 0";
        return;
    }

    s_irradianceSize = value;
}

void IBLCalculator::setIrradianceSapmlesDeltaAngle(float value)
{
    if (value == 0.f)
    {
        LOG_ERROR << "Samples delta angle of irradiance image can't be 0.0";
        return;
    }

    s_irradianceSapmlesDeltaAngle = value;
}

void IBLCalculator::setRadianceSize(uint32_t value)
{
    if (value == 0u)
    {
        LOG_ERROR << "Size of radiance image can't be 0";
        return;
    }

    s_radianceSize = value;
}

void IBLCalculator::setRadianceSamplesCount(uint32_t value)
{
    if (value == 0u)
    {
        LOG_ERROR << "Samples count of radiance image can't be 0";
        return;
    }

    s_radianceSamplesCount = value;
}

}
}
