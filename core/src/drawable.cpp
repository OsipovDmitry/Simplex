#include <core/drawable.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

Drawable::Drawable(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
    : m_(std::make_unique<DrawablePrivate>(mesh, material))
{
}

Drawable::~Drawable() = default;

std::shared_ptr<const Mesh> Drawable::mesh() const
{
    return m().mesh();
}

void Drawable::setMesh(const std::shared_ptr<const Mesh>& value)
{
    auto mPrivate = m();
    mPrivate.mesh() = value;
    mPrivate.onChanged();
}

std::shared_ptr<const Material> Drawable::material() const
{
    return m().material();
}

void Drawable::setMaterial(const std::shared_ptr<const Material>& value)
{
    auto mPrivate = m();
    m().material() = value;
    mPrivate.onChanged();
}

}
}
