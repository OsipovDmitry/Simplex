#ifndef UTILS_IMAGE_H
#define UTILS_IMAGE_H

#include <memory>
#include <filesystem>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/enumclass.h>

namespace simplex
{
namespace utils
{

ENUMCLASS(PixelComponentType, uint16_t,
          Undefined, Single, Uint8, Uint16)
size_t UTILS_SHARED_EXPORT sizeOfPixelComponentType(PixelComponentType value);

class UTILS_SHARED_EXPORT Image final
{
    NONCOPYBLE(Image)
public:
    Image();
    ~Image();

    uint32_t width() const;
    uint32_t height() const;
    uint32_t numComponents() const;
    PixelComponentType type() const;
    const uint8_t *data() const;
    uint8_t *data();

    const uint8_t *pixel(uint32_t, uint32_t) const;
    void setPixel(uint32_t, uint32_t, const uint8_t*);

    std::shared_ptr<Image> convert(uint32_t, uint32_t, uint32_t, PixelComponentType) const;

    bool saveToFile(const std::filesystem::path&);

    static std::shared_ptr<Image> loadFromData(uint32_t width,
                                               uint32_t height,
                                               uint32_t numComponents,
                                               PixelComponentType componentType,
                                               const uint8_t *data);
    static std::shared_ptr<Image> loadFromFile(const std::filesystem::path&);

private:
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_numComponents;
    PixelComponentType m_componentType;
    uint8_t *m_data;

};

}
}

#endif // UTILS_IMAGE_H
