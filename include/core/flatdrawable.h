#ifndef CORE_FLATDRAWABLE_H
#define CORE_FLATDRAWABLE_H

#include <utils/glm/vec4.hpp>

#include <core/visualdrawable.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT FlatDrawable : public VisualDrawable
{
public:
    FlatDrawable(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);
    ~FlatDrawable() override;

    const glm::vec4 &color() const;
    void setColor(const glm::vec4&);

    graphics::PConstTexture colorMap() const;
    void setColorMap(const graphics::PConstTexture&);
};

}
}

#endif // CORE_FLATDRAWABLE_H
