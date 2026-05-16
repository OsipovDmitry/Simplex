#include <vector>

#include <utils/rectpacker.h>

#include "smol-atlas/smol-atlas.h"

namespace simplex
{
namespace utils
{

struct RectPackerPrivate
{
    smol_atlas_t* atlas = nullptr;
    std::vector<smol_atlas_item_t*> items;
    IDsGenerator generator;
};

RectPacker::RectPacker(const glm::uvec2& size)
    : m_(std::make_unique<RectPackerPrivate>())
{
    m_->atlas = sma_atlas_create(static_cast<int>(size.x), static_cast<int>(size.y));
}

RectPacker::~RectPacker()
{
    sma_atlas_destroy(m_->atlas);
}

glm::uvec2 RectPacker::size() const
{
    return glm::uvec2(static_cast<uint32_t>(sma_atlas_width(m_->atlas)), static_cast<uint32_t>(sma_atlas_height(m_->atlas)));
}

RectPacker::ItemID RectPacker::addItem(const glm::uvec2& size)
{
    auto* item = sma_item_add(m_->atlas, static_cast<int>(size.x), static_cast<int>(size.y));
    if (!item) return IDsGenerator::last();

    auto id = m_->generator.generate();
    if (id >= m_->items.size()) m_->items.resize(static_cast<size_t>(id) + 1u);
    m_->items[id] = item;

    return id;
}

bool RectPacker::removeItem(ItemID ID)
{
    if (ID >= m_->items.size()) return false;

    auto* item = m_->items[ID];
    if (item == nullptr) return false;

    sma_item_remove(m_->atlas, item);
    m_->generator.clear(ID);
    m_->items[ID] = nullptr;

    return true;
}

glm::uvec2 RectPacker::itemPosition(ItemID ID) const
{
    if (ID >= m_->items.size()) return glm::uvec2(IDsGenerator::last());

    auto* item = m_->items[ID];
    if (item == nullptr) return glm::uvec2(IDsGenerator::last());

    return glm::uvec2(static_cast<uint32_t>(sma_item_x(item)), static_cast<uint32_t>(sma_item_y(item)));
}

glm::uvec2 RectPacker::itemSize(ItemID ID) const
{
    if (ID >= m_->items.size()) return glm::uvec2(IDsGenerator::last());

    auto* item = m_->items[ID];
    if (item == nullptr) return glm::uvec2(IDsGenerator::last());

    return glm::uvec2(static_cast<uint32_t>(sma_item_width(item)), static_cast<uint32_t>(sma_item_height(item)));
}

} // namespace utils
} // namespace simplex
