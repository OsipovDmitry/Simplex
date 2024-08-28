#include <cstring>

#include <utils/logger.h>
#include <utils/fileextension.h>
#include <utils/image.h>

#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace simplex
{
namespace utils
{

size_t sizeOfPixelComponentType(PixelComponentType value)
{
    static std::array<uint32_t, numElementsPixelComponentType()> s_table {
        0,
        sizeof(float),
        sizeof(uint8_t),
        sizeof(uint16_t)
    };
    return s_table[castFromPixelComponentType(value)];
}

Image::Image()
    : m_width(0u)
    , m_height(0u)
    , m_numComponents(0u)
    , m_componentType(PixelComponentType::Undefined)
    , m_data(nullptr)
{
}

Image::~Image()
{
    stbi_image_free(m_data);
}

uint32_t Image::width() const
{
    return m_width;
}

uint32_t Image::height() const
{
    return m_height;
}

uint32_t Image::numComponents() const
{
    return m_numComponents;
}

PixelComponentType Image::type() const
{
    return m_componentType;
}

const void *Image::data() const
{
    return m_data;
}

void *Image::data()
{
    return m_data;
}

bool Image::saveToFile(const std::filesystem::path &filename)
{
    const auto filenameUtf8 = filename.string();
    auto extension = fileExtension(filename);

    bool result = true;

    if (extension == L".png")
        result = stbi_write_png(filenameUtf8.c_str(),
                                static_cast<int>(width()),
                                static_cast<int>(height()),
                                static_cast<int>(numComponents()),
                                data(),
                                static_cast<int>(width() * numComponents() * sizeOfPixelComponentType(type())));
    else if (extension == L".bmp")
        result = stbi_write_bmp(filenameUtf8.c_str(),
                                static_cast<int>(width()),
                                static_cast<int>(height()),
                                static_cast<int>(numComponents()),
                                data());
    else if (extension == L".tga")
        result = stbi_write_tga(filenameUtf8.c_str(),
                                static_cast<int>(width()),
                                static_cast<int>(height()),
                                static_cast<int>(numComponents()),
                                data());
    else if (extension == L".jpg" || extension == L".jpeg")
        result = stbi_write_jpg(filenameUtf8.c_str(),
                                static_cast<int>(width()),
                                static_cast<int>(height()),
                                static_cast<int>(numComponents()),
                                data(),
                                100);
    else if (extension == L".hdr")
        result = stbi_write_hdr(filenameUtf8.c_str(),
                                static_cast<int>(width()),
                                static_cast<int>(height()),
                                static_cast<int>(numComponents()),
                                reinterpret_cast<const float*>(data()));
    else
    {
        LOG_ERROR << "Unsupported format <\"" << extension << "\"";
        return false;
    }

    if (!result)
        LOG_ERROR << "Failed to save image " << filename;

    return result;
}

std::shared_ptr<Image> Image::loadFromData(uint32_t width,
                                           uint32_t height,
                                           uint32_t numComponents,
                                           PixelComponentType componentType,
                                           const void *data)
{
    if (width * height == 0)
        LOG_CRITICAL << "Width and height can't be 0";

    if ((numComponents < 1) ||  (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (componentType == PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto result = std::make_shared<Image>();
    result->m_width = width;
    result->m_height = height;
    result->m_numComponents = numComponents;
    result->m_componentType = componentType;

    size_t dataSize = width * height * numComponents * sizeOfPixelComponentType(componentType);
    result->m_data = std::malloc(dataSize);

    if (data)
        std::memcpy(result->m_data,  data, dataSize);

    return result;
}

std::shared_ptr<Image> Image::loadFromFile(const std::filesystem::path &filename)
{
    int w, h, n;
    void *d;
    PixelComponentType t;

    const auto filenameUtf8 = filename.string();

    if (stbi_is_hdr(filenameUtf8.c_str()))
    {
        d = stbi_loadf(filenameUtf8.c_str(), &w, &h, &n, 0);
        t = PixelComponentType::Single;
    }
    else if (stbi_is_16_bit(filenameUtf8.c_str()))
    {
        d = stbi_load_16(filenameUtf8.c_str(), &w, &h, &n, 0);
        t = PixelComponentType::Uint16;
    }
    else
    {
        d = stbi_load(filenameUtf8.c_str(), &w, &h, &n, 0);
        t = PixelComponentType::Uint8;
    }

    if (!d)
        return nullptr;

    auto result = std::make_shared<Image>();
    result->m_width = static_cast<uint32_t>(w);
    result->m_height = static_cast<uint32_t>(h);
    result->m_numComponents = static_cast<uint32_t>(n);
    result->m_data = d;
    result->m_componentType = t;

    return result;
}

}
}
