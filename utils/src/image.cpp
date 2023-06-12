#include <utils/image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace simplex
{
namespace utils
{

Image::Image()
    : m_width(0u)
    , m_height(0u)
    , m_numComponents(0u)
    , m_type(Type::Undefined)
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

Type Image::type() const
{
    return m_type;
}

const void *Image::data() const
{
    return m_data;
}

std::shared_ptr<Image> Image::loadImage(const std::string &filename)
{
    int w, h, n;
    void *d;
    Type t;

    if (stbi_is_hdr(filename.c_str()))
    {
        d = stbi_loadf(filename.c_str(), &w, &h, &n, 0);
        t = Type::Single;
    }
    else if (stbi_is_16_bit(filename.c_str()))
    {
        d = stbi_load_16(filename.c_str(), &w, &h, &n, 0);
        t = Type::Uint16;
    }
    else
    {
        d = stbi_load(filename.c_str(), &w, &h, &n, 0);
        t = Type::Uint8;
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
