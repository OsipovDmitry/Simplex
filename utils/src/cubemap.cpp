#include <cstring>

#include <utils/glm/gtc/constants.hpp>
#include <utils/glm/common.hpp>
#include <utils/glm/gtx/polar_coordinates.hpp>

#include <utils/logger.h>
#include <utils/image.h>
#include <utils/cubemap.h>

namespace simplex
{
namespace utils
{

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

    if (image->numComponents() < 1u || image->numComponents() > 4u)
    {
        LOG_ERROR << "omponents count of image must be [1..4]";
        return false;
    }

    if (image->type() == PixelComponentType::Count)
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

inline void spherical2Face(const std::shared_ptr<Image> &src,
                           std::shared_ptr<Image> &dst,
                           uint32_t faceSize,
                           uint32_t dstXOffset,
                           uint32_t dstYOffset,
                           const glm::vec3 &euclideanXAxis,
                           const glm::vec3 &euclideanYAxis,
                           const glm::vec3 &euclideanZAxis)
{
    const size_t pixelSize = src->numComponents() * sizeOfPixelComponentType(src->type());
    const size_t dstLineSize = dst->width() * pixelSize;

    for (uint32_t y = 0u; y < faceSize; ++y)
    {
        const size_t dstLineOffset = dstLineSize * (dstYOffset + y);
        for (uint32_t x = 0u; x < faceSize; ++x)
        {
            const float xf = static_cast<float>(x) / static_cast<float>(faceSize - 1u) - .5f;
            const float yf = static_cast<float>(y) / static_cast<float>(faceSize - 1u) - .5f;

            auto polar = glm::polar(euclideanXAxis * xf + euclideanYAxis * yf + euclideanZAxis);
            const float latitude = glm::clamp(.5f + polar.x / glm::pi<float>(), 0.f, 1.f);
            const float longitude = glm::clamp(.5f - polar.y / glm::two_pi<float>(), 0.f, 1.f);

            const auto srcX = static_cast<uint32_t>(longitude * static_cast<float>(src->width() - 1u) + .5f);
            const auto srcY = static_cast<uint32_t>(latitude * static_cast<float>(src->height() - 1u) + .5f);

            const auto *srcPixel = static_cast<const uint8_t*>(src->data()) + pixelSize * (src->width() * srcY + srcX);
            auto *dstPixel = static_cast<uint8_t*>(dst->data()) + dstLineOffset + pixelSize * (dstXOffset + x);

            std::memcpy(dstPixel, srcPixel, pixelSize);
        }
    }
}

std::shared_ptr<Image> CubemapConverter::convertSphericalCubemapToHCross(const std::shared_ptr<Image> &src)
{
    static const std::array<uint32_t, 6u> faceXOffsets { 0u, 2u, 1u, 1u, 1u, 3u };
    static const std::array<uint32_t, 6u> faceYOffsets { 1u, 1u, 0u, 2u, 1u, 1u };

    static const std::array<std::array<glm::vec3, 3u>, 6u> euclideanAxes {
        { {glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(.5f, 0.f, 0.f)},
          {glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(-.5f, 0.f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -.5f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, .5f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, .5f)},
          {glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -.5f)}}
    };

    if (!checkImage(src))
        return nullptr;

    std::shared_ptr<Image> result;

    const uint32_t faceSize = src->height() / 2u;
    result = std::make_shared<Image>(faceSize * 4u, faceSize * 3u, src->numComponents(), src->type(), nullptr);

    for (uint32_t face = 0u; face < 6u; ++face)
        spherical2Face(src,
                       result,
                       faceSize,
                       faceXOffsets[face] * faceSize,
                       faceYOffsets[face] * faceSize,
                       euclideanAxes[face][0u],
                       euclideanAxes[face][1u],
                       euclideanAxes[face][2u]);

    return result;
}

std::shared_ptr<Image> CubemapConverter::convertSphericalCubemapToVCross(const std::shared_ptr<Image> &src)
{
    static const std::array<uint32_t, 6u> faceXOffsets { 0u, 2u, 1u, 1u, 1u, 1u };
    static const std::array<uint32_t, 6u> faceYOffsets { 1u, 1u, 0u, 2u, 1u, 3u };

    static const std::array<std::array<glm::vec3, 3u>, 6u> euclideanAxes {
        { {glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(.5f, 0.f, 0.f)},
          {glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(-.5f, 0.f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -.5f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, .5f, 0.f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, .5f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -.5f)}}
    };

    if (!checkImage(src))
        return nullptr;

    std::shared_ptr<Image> result;

    const uint32_t faceSize = src->height() / 2u;
    result = std::make_shared<Image>(faceSize * 3u, faceSize * 4u, src->numComponents(), src->type(), nullptr);

    for (uint32_t face = 0u; face < 6u; ++face)
        spherical2Face(src,
                       result,
                       faceSize,
                       faceXOffsets[face] * faceSize,
                       faceYOffsets[face] * faceSize,
                       euclideanAxes[face][0u],
                       euclideanAxes[face][1u],
                       euclideanAxes[face][2u]);

    return result;
}

std::vector<std::shared_ptr<Image>> CubemapConverter::convertSphericalCubemapToFacesList(const std::shared_ptr<Image> &src)
{
    static const std::array<std::array<glm::vec3, 3u>, 6u> euclideanAxes {
        { {glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(.5f, 0.f, 0.f)},
          {glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(-.5f, 0.f, 0.f)},
          {glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -.5f, 0.f)},
          {glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, .5f, 0.f)},
          {glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, .5f)},
          {glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -.5f)}}
    };

    if (!checkImage(src))
        return std::vector<std::shared_ptr<Image>>();

    std::vector<std::shared_ptr<Image>> result(euclideanAxes.size());

    const uint32_t faceSize = glm::max(src->height() / 2u, 1u);

    for (uint32_t face = 0u; face < result.size(); ++face)
    {
        result[face] = std::make_shared<Image>(faceSize, faceSize, src->numComponents(), src->type(), nullptr);
        spherical2Face(src,
                       result[face],
                       faceSize,
                       0u,
                       0u,
                       euclideanAxes[face][0u],
                       euclideanAxes[face][1u],
                       euclideanAxes[face][2u]);
    }

    return result;
}

}
}
