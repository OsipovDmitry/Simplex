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

class UTILS_SHARED_EXPORT Image
{
    NONCOPYBLE(Image)
public:
    explicit Image();
    ~Image();

    uint32_t width() const;
    uint32_t height() const;
    uint32_t numComponents() const;
    PixelComponentType type() const;
    const void *data() const;

    static std::shared_ptr<Image> createImage(uint32_t, uint32_t, uint32_t, PixelComponentType, const void*);
    static std::shared_ptr<Image> loadImage(const std::filesystem::path&);

private:
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_numComponents;
    PixelComponentType m_type;
    void *m_data;

};

}
}

#endif // UTILS_IMAGE_H
