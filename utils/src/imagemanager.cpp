#include <filesystem>

#include <utils/logger.h>
#include <utils/fileextension.h>
#include <utils/image.h>
#include <utils/imagemanager.h>

#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace simplex
{
namespace utils
{

ImageManager::~ImageManager() = default;

std::shared_ptr<Image> ImageManager::loadOrGet(const std::filesystem::path& filename)
{
    const auto absoluteFilename = std::filesystem::absolute(filename);

    if (auto it = m_images.find(absoluteFilename); it != m_images.end())
        return it->second;

    int w, h, n;
    uint8_t* d;
    PixelComponentType t;

    const auto filenameUtf8 = filename.string();

    if (stbi_is_hdr(filenameUtf8.c_str()))
    {
        d = reinterpret_cast<uint8_t*>(stbi_loadf(filenameUtf8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Single;
    }
    else if (stbi_is_16_bit(filenameUtf8.c_str()))
    {
        d = reinterpret_cast<uint8_t*>(stbi_load_16(filenameUtf8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Uint16;
    }
    else
    {
        d = reinterpret_cast<uint8_t*>(stbi_load(filenameUtf8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Uint8;
    }

    if (!d)
    {
        LOG_ERROR << "Failed to load image " << absoluteFilename;
        return nullptr;
    }

    auto result = std::shared_ptr<Image>(new Image);
    result->m_width = static_cast<uint32_t>(w);
    result->m_height = static_cast<uint32_t>(h);
    result->m_numComponents = static_cast<uint32_t>(n);
    result->m_data = d;
    result->m_componentType = t;

    m_images.insert({ absoluteFilename, result });
    return result;
}

std::shared_ptr<Image> ImageManager::loadOrGetDescription(const std::filesystem::path& filename)
{
    const auto absoluteFilename = std::filesystem::absolute(filename);

    if (auto it = m_images.find(absoluteFilename); it != m_images.end())
        return it->second;

    int w, h, n;
    const auto filenameUtf8 = filename.string();

    if (!stbi_info(filenameUtf8.c_str(), &w, &h, &n))
    {
        LOG_ERROR << "Failed to load image description " << absoluteFilename;
        return nullptr;
    }

    auto result = std::shared_ptr<Image>(new Image);
    result->m_width = static_cast<uint32_t>(w);
    result->m_height = static_cast<uint32_t>(h);
    result->m_numComponents = static_cast<uint32_t>(n);
    result->m_data = nullptr;
    result->m_componentType = PixelComponentType::Count;

    return result;
}

ImageManager::ImageManager()
{}


}
}
