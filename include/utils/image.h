#ifndef UTILS_IMAGE_H
#define UTILS_IMAGE_H

#include <memory>
#include <string>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/types.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT Image
{
    NONCOPYBLE(Image)
public:
    explicit Image();
    ~Image();

    uint32_t width() const;
    uint32_t height() const;
    uint32_t numComponents() const;
    Type type() const;
    const void *data() const;

    static std::shared_ptr<Image> loadImage(const std::string&);

private:
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_numComponents;
    Type m_type;
    void *m_data;

};

}
}

#endif // UTILS_IMAGE_H
