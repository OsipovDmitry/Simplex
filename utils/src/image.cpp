#include <cstring>

#include <utils/image.h>

#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace simplex
{
namespace utils
{

inline size_t sizeOfPixelComponentType(PixelComponentType value)
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
    , m_type(PixelComponentType::Undefined)
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
    return m_type;
}

const void *Image::data() const
{
    return m_data;
}

std::shared_ptr<Image> Image::loadFromData(uint32_t w, uint32_t h, uint32_t n, PixelComponentType t, const void *d)
{
    assert(w > 0);
    assert(h > 0);
    assert((n > 0) && (n <= 4));
    assert(t != PixelComponentType::Undefined);

    auto result = std::make_shared<Image>();
    result->m_width = w;
    result->m_height = h;
    result->m_numComponents = n;
    result->m_type = t;

    size_t dataSize = w * h * n * sizeOfPixelComponentType(t);
    result->m_data = std::malloc(dataSize);

    if (d)
        std::memcpy(result->m_data,  d, dataSize);

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
    result->m_type = t;

    return result;
}

}
}
