#include <array>

#include <utils/logger.h>
#include <utils/image.h>
#include <utils/fileextension.h>

#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "imageimpl.h"

namespace simplex
{
namespace utils
{

size_t sizeOfPixelComponentType(PixelComponentType value)
{
    static std::array<uint32_t, numElementsPixelComponentType()> s_table {
        sizeof(float),
        sizeof(uint8_t),
        sizeof(uint16_t)
    };
    return s_table[castFromPixelComponentType(value)];
}

Image::Image(uint32_t width, uint32_t height, uint32_t numComponents)
    : m_width(width)
    , m_height(height)
    , m_numComponents(numComponents)
    , m_componentType(PixelComponentType::Count)
    , m_data(nullptr)
{
    if (!m_width)
        LOG_CRITICAL << "Width can't be 0";

    if (!m_height)
        LOG_CRITICAL << "Height can't be 0";

    if ((m_numComponents < 1) || (m_numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";
}

Image::Image(uint32_t width, uint32_t height, uint32_t numComponents, PixelComponentType componentType, const uint8_t* data)
    : m_width(width)
    , m_height(height)
    , m_numComponents(numComponents)
    , m_componentType(componentType)
    , m_data(nullptr)
{
    if (!m_width)
        LOG_CRITICAL << "Width can't be 0";

    if (!m_height)
        LOG_CRITICAL << "Height can't be 0";

    if ((m_numComponents < 1) || (m_numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (m_componentType == PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    const size_t dataSize = sizeOfPixelComponentType(componentType) * width * height * numComponents;
    m_data = reinterpret_cast<uint8_t*>(std::malloc(dataSize));

    if (data)
        std::memcpy(m_data, data, dataSize);
}

Image::~Image()
{
    std::free(m_data);
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

const uint8_t *Image::data() const
{
    return m_data;
}

uint8_t *Image::data()
{
    return m_data;
}

const uint8_t* Image::pixel(uint32_t x, uint32_t y) const
{
    const size_t pixelSize = m_numComponents * sizeOfPixelComponentType(m_componentType);
    return m_data + pixelSize * (m_width * y + x);
}

void Image::setPixel(uint32_t x, uint32_t y, const uint8_t *data)
{
    const size_t pixelSize = m_numComponents * sizeOfPixelComponentType(m_componentType);
    std::memcpy(m_data + pixelSize * (m_width * y + x),
        data,
        pixelSize);
}

std::shared_ptr<Image> Image::copy() const
{
    return std::make_shared<Image>(m_width, m_height, m_numComponents, m_componentType, m_data);
}

void Image::convert(
    uint32_t width,
    uint32_t height,
    uint32_t numComponents,
    PixelComponentType componentType)
{
    if (width * height == 0)
    {
        LOG_CRITICAL << "Width and height can't be 0";
        return;
    }

    if ((numComponents < 1) || (numComponents > 4))
    {
        LOG_CRITICAL << "Num components must be in [1..4]";
        return;
    }

    if (componentType == PixelComponentType::Count)
    {
        LOG_CRITICAL << "Undefined pixel component type";
        return;
    }

    if (m_componentType != componentType)
    {
        if (auto data = convertToPixelComponentType(m_width, m_height, m_numComponents, m_componentType, m_data, componentType))
        {
            std::free(m_data);
            m_data = data;
        }
    }

    if (m_numComponents != numComponents)
    {
        if (auto data = convertToNumComponents(m_width, m_height, m_numComponents, m_componentType, m_data, numComponents))
        {
            std::free(m_data);
            m_data = data;
        }
    }

    if ((m_width != width) && (m_height != height))
    {
        if (auto data = convertToSize(m_width, m_height, m_numComponents, m_componentType, m_data, width, height))
        {
            std::free(m_data);
            m_data = data;
        }
    }
}

std::shared_ptr<Image> Image::converted(
    uint32_t width,
    uint32_t height,
    uint32_t numComponents,
    PixelComponentType componentType) const
{
    auto result = copy();
    result->convert(width, height, numComponents, componentType);
    return result;
}

std::shared_ptr<Image> Image::loadFromFile(const std::filesystem::path& fileName)
{
    const auto absoluteFileName = std::filesystem::canonical(fileName);

    int w, h, n;
    uint8_t* d;
    PixelComponentType t;

    const auto filenameUTF8 = absoluteFileName.string();
    if (stbi_is_hdr(filenameUTF8.c_str()))
    {
        d = reinterpret_cast<uint8_t*>(stbi_loadf(filenameUTF8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Single;
    }
    else if (stbi_is_16_bit(filenameUTF8.c_str()))
    {
        d = reinterpret_cast<uint8_t*>(stbi_load_16(filenameUTF8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Uint16;
    }
    else
    {
        d = reinterpret_cast<uint8_t*>(stbi_load(filenameUTF8.c_str(), &w, &h, &n, 0));
        t = PixelComponentType::Uint8;
    }

    if (!d)
        return nullptr;

    return std::make_shared<Image>(
        static_cast<uint32_t>(w),
        static_cast<uint32_t>(h),
        static_cast<uint32_t>(n),
        t,
        d);
}

std::shared_ptr<Image> Image::loadDescriptionFromFile(const std::filesystem::path& fileName)
{
    const auto absoluteFileName = std::filesystem::canonical(fileName);

    int w, h, n;
    const auto fileNameUtf8 = absoluteFileName.string();

    if (!stbi_info(fileNameUtf8.c_str(), &w, &h, &n))
    {
        return nullptr;
    }

    return std::make_shared<Image>(
        static_cast<uint32_t>(w),
        static_cast<uint32_t>(h),
        static_cast<uint32_t>(n));
}

bool Image::saveToFile(const std::filesystem::path &fileName) const
{
    const auto fileNameUTF8 = fileName.string();
    const auto extension = fileExtension(fileName);

    bool result = true;

    if (extension == L".png")
    {
        result = stbi_write_png(fileNameUTF8.c_str(),
            static_cast<int>(width()),
            static_cast<int>(height()),
            static_cast<int>(numComponents()),
            data(),
            static_cast<int>(width() * numComponents() * sizeOfPixelComponentType(type())));
    }
    else if (extension == L".bmp")
    {
        result = stbi_write_bmp(fileNameUTF8.c_str(),
            static_cast<int>(width()),
            static_cast<int>(height()),
            static_cast<int>(numComponents()),
            data());
    }
    else if (extension == L".tga")
    {
        result = stbi_write_tga(fileNameUTF8.c_str(),
            static_cast<int>(width()),
            static_cast<int>(height()),
            static_cast<int>(numComponents()),
            data());
    }
    else if (extension == L".jpg" || extension == L".jpeg")
    {
        result = stbi_write_jpg(fileNameUTF8.c_str(),
            static_cast<int>(width()),
            static_cast<int>(height()),
            static_cast<int>(numComponents()),
            data(),
            100);
    }
    else if (extension == L".hdr")
    {
        result = stbi_write_hdr(fileNameUTF8.c_str(),
            static_cast<int>(width()),
            static_cast<int>(height()),
            static_cast<int>(numComponents()),
            reinterpret_cast<const float*>(data()));
    }
    else
    {
        LOG_ERROR << "Unsupported image format <\"" << extension << "\"";
        return false;
    }

    if (!result)
        LOG_ERROR << "Failed to save image " << fileName;

    return result;
}

}
}
