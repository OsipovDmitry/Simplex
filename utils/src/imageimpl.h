#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

#include <utils/glm/detail/type_vec1.hpp>
#include <utils/glm/detail/type_vec2.hpp>
#include <utils/glm/detail/type_vec3.hpp>
#include <utils/glm/detail/type_vec4.hpp>

namespace simplex
{
namespace utils
{

template<typename DstType, typename SrcType>
DstType convertToPixelComponentType(SrcType value)
{
    return static_cast<DstType>(0);
}

template<> float convertToPixelComponentType<float, uint8_t>(uint8_t value)
{
    return static_cast<float>(value) / std::numeric_limits<uint8_t>::max();
}

template<> float convertToPixelComponentType<float, uint16_t>(uint16_t value)
{
    return static_cast<float>(value) / std::numeric_limits<uint16_t>::max();
}

template<> uint8_t convertToPixelComponentType<uint8_t, uint16_t>(uint16_t value)
{
    return static_cast<uint8_t>(value >> 8u);
}

template<> uint8_t convertToPixelComponentType<uint8_t, float>(float value)
{
    return static_cast<uint8_t>(glm::clamp(value, 0.f, 1.f) * std::numeric_limits<uint8_t>::max());
}

template<> uint16_t convertToPixelComponentType<uint16_t, float>(float value)
{
    return static_cast<uint16_t>(glm::clamp(value, 0.f, 1.f) * std::numeric_limits<uint16_t>::max());
}

template<> uint16_t convertToPixelComponentType<uint16_t, uint8_t>(uint8_t value)
{
    return static_cast<uint16_t>(value << 8u);
}

template<typename DstType, typename SrcType>
DstType* convertToPixelComponentType(uint32_t w, uint32_t h, uint32_t n, const SrcType* d)
{
    DstType* result = nullptr;

    if constexpr (!std::is_same<DstType, SrcType>())
    {
        result = reinterpret_cast<DstType*>(STBI_MALLOC(w * h * n * sizeof(DstType)));

        const size_t total = static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(n);
        for (size_t i = 0u; i < total; ++i)
            result[i] = convertToPixelComponentType<DstType, SrcType>(d[i]);
    }

    return result;
}

template<typename DstType>
uint8_t* convertToPixelComponentType(uint32_t w, uint32_t h, uint32_t n, PixelComponentType pct, const uint8_t* d)
{
    DstType* result = nullptr;

    switch (pct)
    {
    case PixelComponentType::Single:
    {
        result = convertToPixelComponentType<DstType, float>(w, h, n, reinterpret_cast<const float*>(d));
        break;
    }
    case PixelComponentType::Uint8:
    {
        result = convertToPixelComponentType<DstType, uint8_t>(w, h, n, reinterpret_cast<const uint8_t*>(d));
        break;
    }
    case PixelComponentType::Uint16:
    {
        result = convertToPixelComponentType<DstType, uint16_t>(w, h, n, reinterpret_cast<const uint16_t*>(d));
        break;
    }
    default:
        break;
    }

    return reinterpret_cast<uint8_t*>(result);
}

static uint8_t* convertToPixelComponentType(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    PixelComponentType& pct,
    const uint8_t* d,
    PixelComponentType newPCT)
{
    uint8_t* result = nullptr;

    switch (newPCT)
    {
    case PixelComponentType::Single:
    {
        result = convertToPixelComponentType<float>(w, h, n, pct, d);
        break;
    }
    case PixelComponentType::Uint8:
    {
        result = convertToPixelComponentType<uint8_t>(w, h, n, pct, d);
        break;
    }
    case PixelComponentType::Uint16:
    {
        result = convertToPixelComponentType<uint16_t>(w, h, n, pct, d);
        break;
    }
    default:
        break;
    }

    pct = newPCT;
    return result;
}

template<typename T>
T zero() { return static_cast<T>(0); }
template<> float zero<float>() { return 0.f; }
template<> uint8_t zero<uint8_t>() { return 0u; }
template<> uint16_t zero<uint16_t>() { return 0u; }

template<typename T>
T one() { return static_cast<T>(1); }
template<> float one<float>() { return 1.f; }
template<> uint8_t one<uint8_t>() { return std::numeric_limits<uint8_t>::max(); }
template<> uint16_t one<uint16_t>() { return std::numeric_limits<uint16_t>::max(); }

template<glm::length_t Dst, glm::length_t Src, typename T>
struct ConvertToNumComponents
{
    glm::vec<Dst, T> operator()(const glm::vec<Src, T>&) const { return glm::vec<Dst, T>(); }
};

template<typename T>
struct ConvertToNumComponents<1, 1, T>
{
    glm::vec<1, T> operator()(const glm::vec<1, T>& value) const { return glm::vec<1, T>(value.r); }
};

template<typename T>
struct ConvertToNumComponents<1, 2, T>
{
    glm::vec<1, T> operator()(const glm::vec<2, T>& value) const { return glm::vec<1, T>(value.r); }
};

template<typename T>
struct ConvertToNumComponents<1, 3, T>
{
    glm::vec<1, T> operator()(const glm::vec<3, T>& value) const { return glm::vec<1, T>(value.r); }
};

template<typename T>
struct ConvertToNumComponents<1, 4, T>
{
    glm::vec<1, T> operator()(const glm::vec<4, T>& value) const { return glm::vec<1, T>(value.r); }
};



template<typename T>
struct ConvertToNumComponents<2, 1, T>
{
    glm::vec<2, T> operator()(const glm::vec<1, T>& value) const { return glm::vec<2, T>(value.r, zero<T>()); }
};

template<typename T>
struct ConvertToNumComponents<2, 2, T>
{
    glm::vec<2, T> operator()(const glm::vec<2, T>& value) const { return glm::vec<2, T>(value.r, value.g); }
};

template<typename T>
struct ConvertToNumComponents<2, 3, T>
{
    glm::vec<2, T> operator()(const glm::vec<3, T>& value) const { return glm::vec<2, T>(value.r, value.g); }
};

template<typename T>
struct ConvertToNumComponents<2, 4, T>
{
    glm::vec<2, T> operator()(const glm::vec<4, T>& value) const { return glm::vec<2, T>(value.r, value.g); }
};



template<typename T>
struct ConvertToNumComponents<3, 1, T>
{
    glm::vec<3, T> operator()(const glm::vec<1, T>& value) const { return glm::vec<3, T>(value.r, zero<T>(), zero<T>()); }
};

template<typename T>
struct ConvertToNumComponents<3, 2, T>
{
    glm::vec<3, T> operator()(const glm::vec<2, T>& value) const { return glm::vec<3, T>(value.r, value.g, zero<T>()); }
};

template<typename T>
struct ConvertToNumComponents<3, 3, T>
{
    glm::vec<3, T> operator()(const glm::vec<3, T>& value) const { return glm::vec<3, T>(value.r, value.g, value.b); }
};

template<typename T>
struct ConvertToNumComponents<3, 4, T>
{
    glm::vec<3, T> operator()(const glm::vec<4, T>& value) const { return glm::vec<3, T>(value.r, value.g, value.b); }
};



template<typename T>
struct ConvertToNumComponents<4, 1, T>
{
    glm::vec<4, T> operator()(const glm::vec<1, T>& value) const { return glm::vec<4, T>(value.r, zero<T>(), zero<T>(), one<T>()); }
};

template<typename T>
struct ConvertToNumComponents<4, 2, T>
{
    glm::vec<4, T> operator()(const glm::vec<2, T>& value) const { return glm::vec<4, T>(value.r, value.g, zero<T>(), one<T>()); }
};

template<typename T>
struct ConvertToNumComponents<4, 3, T>
{
    glm::vec<4, T> operator()(const glm::vec<3, T>& value) const { return glm::vec<4, T>(value.r, value.g, value.b, one<T>()); }
};

template<typename T>
struct ConvertToNumComponents<4, 4, T>
{
    glm::vec<4, T> operator()(const glm::vec<4, T>& value) const { return glm::vec<4, T>(value.r, value.g, value.b, value.a); }
};

template<glm::length_t Dst, glm::length_t Src, typename T>
glm::vec<Dst, T>* convertToNumComponents(uint32_t w, uint32_t h, const glm::vec<Src, T>* d)
{
    glm::vec<Dst, T>* result = nullptr;

    if constexpr (Dst != Src)
    {
        ConvertToNumComponents<Dst, Src, T> converter;

        result = reinterpret_cast<glm::vec<Dst, T>*>(STBI_MALLOC(w * h * Dst * sizeof(T)));

        const size_t total = static_cast<size_t>(w) * static_cast<size_t>(h);
        for (size_t i = 0u; i < total; ++i)
            result[i] = converter(d[i]);
    }

    return result;
}

template<glm::length_t Dst, typename T>
glm::vec<Dst, T>* convertToNumComponents(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const uint8_t* d)
{
    glm::vec<Dst, T>* result = nullptr;

    switch (n)
    {
    case 1:
    {
        result = convertToNumComponents<Dst, 1, T>(w, h, reinterpret_cast<const glm::vec<1, T>*>(d));
        break;
    }
    case 2:
    {
        result = convertToNumComponents<Dst, 2, T>(w, h, reinterpret_cast<const glm::vec<2, T>*>(d));
        break;
    }
    case 3:
    {
        result = convertToNumComponents<Dst, 3, T>(w, h, reinterpret_cast<const glm::vec<3, T>*>(d));
        break;
    }
    case 4:
    {
        result = convertToNumComponents<Dst, 4, T>(w, h, reinterpret_cast<const glm::vec<4, T>*>(d));
        break;
    }
    default:
        break;
    }

    return result;
}

template<typename T>
uint8_t* convertToNumComponents(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const uint8_t* d,
    uint32_t newNumComponents)
{
    uint8_t* result = nullptr;

    switch (newNumComponents)
    {
    case 1:
    {
        result = reinterpret_cast<uint8_t*>(convertToNumComponents<1, T>(w, h, n, d));
        break;
    }
    case 2:
    {
        result = reinterpret_cast<uint8_t*>(convertToNumComponents<2, T>(w, h, n, d));
        break;
    }
    case 3:
    {
        result = reinterpret_cast<uint8_t*>(convertToNumComponents<3, T>(w, h, n, d));
        break;
    }
    case 4:
    {
        result = reinterpret_cast<uint8_t*>(convertToNumComponents<4, T>(w, h, n, d));
        break;
    }
    default:
        break;
    }

    return result;
}

uint8_t* convertToNumComponents(
    uint32_t w,
    uint32_t h,
    uint32_t& n,
    PixelComponentType pct,
    const uint8_t* d,
    uint32_t newN)
{
    uint8_t* result = nullptr;

    switch (pct)
    {
    case PixelComponentType::Single:
    {
        result = convertToNumComponents<float>(w, h, n, d, newN);
        break;
    }
    case PixelComponentType::Uint8:
    {
        result = convertToNumComponents<uint8_t>(w, h, n, d, newN);
        break;
    }
    case PixelComponentType::Uint16:
    {
        result = convertToNumComponents<uint16_t>(w, h, n, d, newN);
        break;
    }
    default:
        break;
    }

    n = newN;
    return result;
}

template<typename T>
T* convertToSize(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const T* d,
    uint32_t newW,
    uint32_t newH)
{
    return nullptr;
}

template<> float* convertToSize(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const float* d,
    uint32_t newW,
    uint32_t newH)
{
    void* tmp;
    auto result = reinterpret_cast<float*>(STBI_MALLOC(newW * newH * n * sizeof(float)));
    if (!stbir_resize_float_generic(
        d, w, h, 0,
        result, newW, newH, 0,
        n,
        n == 4u ? 3u : STBIR_ALPHA_CHANNEL_NONE,
        0,
        STBIR_EDGE_CLAMP,
        STBIR_FILTER_DEFAULT,
        STBIR_COLORSPACE_LINEAR,
        tmp))
    {
        stbi_image_free(result);
        result = nullptr;
    }
    return result;
}

template<> uint8_t* convertToSize(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const uint8_t* d,
    uint32_t newW,
    uint32_t newH)
{
    void* tmp = nullptr;
    auto result = reinterpret_cast<uint8_t*>(STBI_MALLOC(newW * newH * n * sizeof(uint8_t)));
    if (!stbir_resize_uint8_generic(
        d, w, h, 0,
        result, newW, newH, 0,
        n,
        n == 4u ? 3u : STBIR_ALPHA_CHANNEL_NONE,
        0,
        STBIR_EDGE_CLAMP,
        STBIR_FILTER_DEFAULT,
        STBIR_COLORSPACE_LINEAR,
        tmp))
    {
        stbi_image_free(result);
        result = nullptr;
    }
    return result;
}

template<> uint16_t* convertToSize(
    uint32_t w,
    uint32_t h,
    uint32_t n,
    const uint16_t* d,
    uint32_t newW,
    uint32_t newH)
{
    void* tmp;
    auto result = reinterpret_cast<uint16_t*>(STBI_MALLOC(newW * newH * n * sizeof(uint16_t)));
    if (!stbir_resize_uint16_generic(
        d, w, h, 0,
        result, newW, newH, 0,
        n,
        n == 4u ? 3u : STBIR_ALPHA_CHANNEL_NONE,
        0,
        STBIR_EDGE_CLAMP,
        STBIR_FILTER_DEFAULT,
        STBIR_COLORSPACE_LINEAR,
        tmp))
    {
        stbi_image_free(result);
        result = nullptr;
    }
    return result;
}

uint8_t* convertToSize(
    uint32_t& w,
    uint32_t& h,
    uint32_t n,
    PixelComponentType pct,
    const uint8_t* d,
    uint32_t newW,
    uint32_t newH)
{
    uint8_t* result = nullptr;

    if ((newW != w) || (newH != h))
    {
        switch (pct)
        {
        case PixelComponentType::Single:
        {
            result = reinterpret_cast<uint8_t*>(convertToSize<float>(w, h, n, reinterpret_cast<const float*>(d), newW, newH));
            break;
        }
        case PixelComponentType::Uint8:
        {
            result = reinterpret_cast<uint8_t*>(convertToSize<uint8_t>(w, h, n, reinterpret_cast<const uint8_t*>(d), newW, newH));
            break;
        }
        case PixelComponentType::Uint16:
        {
            result = reinterpret_cast<uint8_t*>(convertToSize<uint16_t>(w, h, n, reinterpret_cast<const uint16_t*>(d), newW, newH));
            break;
        }
        default:
            break;
        }
    }

    if (result)
    {
        w = newW;
        h = newH;
    }

    return result;
}

}
}