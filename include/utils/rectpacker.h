#ifndef UTILS_RECT_PACKER_H
#define UTILS_RECT_PACKER_H

#include <memory>

#include <utils/glm/vec2.hpp>
#include <utils/idgenerator.h>
#include <utils/noncopyble.h>
#include <utils/utilsglobal.h>

namespace simplex
{
namespace utils
{

struct RectPackerPrivate;
class UTILS_SHARED_EXPORT RectPacker final
{
    NONCOPYBLE(RectPacker)
public:
    using ItemID = IDsGenerator::value_type;

    RectPacker(const glm::uvec2& size);
    ~RectPacker();

    glm::uvec2 size() const;

    ItemID addItem(const glm::uvec2& size);
    bool removeItem(ItemID ID);

    glm::uvec2 itemPosition(ItemID ID) const;
    glm::uvec2 itemSize(ItemID ID) const;

private:
    std::unique_ptr<RectPackerPrivate> m_;
};

} // namespace utils
} // namespace simplex

#endif // UTILS_RECT_PACKER_H
