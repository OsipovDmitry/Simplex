#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>
#include <utils/mesh.h>

#include <core/graphicsrendererbase.h>
#include <core/drawable.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

Drawable::Drawable(const std::shared_ptr<graphics::IVertexArray> &vertexArray)
    : Drawable(std::make_unique<DrawablePrivate>(vertexArray))
{
}

Drawable::Drawable(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
    : Drawable(std::make_unique<DrawablePrivate>(mesh, material))
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
    return const_cast<Drawable*>(this)->material();
}

void Drawable::setMaterial(const std::shared_ptr<const Material>& value)
{
    auto mPrivate = m();
    m().material() = value;
    mPrivate.onChanged();
}

std::shared_ptr<const graphics::IVertexArray> Drawable::vertexArray() const
{
    return const_cast<Drawable*>(this)->vertexArray();
}

std::shared_ptr<graphics::IVertexArray> Drawable::vertexArray()
{
    return m().vertexArray();
}

void Drawable::setVertexArray(std::shared_ptr<graphics::IVertexArray> value)
{
    m().vertexArray() = value;
}

utils::VertexAttributeSet Drawable::vertexAttrubiteSet() const
{
    utils::VertexAttributeSet result;

    auto vao = vertexArray();
    for (typename std::underlying_type<utils::VertexAttribute>::type i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (auto attrib = utils::castToVertexAttribute(i); vao->vertexAttributeBindingIndex(attrib) != static_cast<uint32_t>(-1))
            result.insert(attrib);

    return result;
}

bool Drawable::isDoubleSided() const
{
    return m().isDoubleSided();
}

void Drawable::setDoubleSided(bool value)
{
    m().isDoubleSided() = value;
}

Drawable::Drawable(std::unique_ptr<DrawablePrivate> drawablePrivate)
    : StateSet(std::move(drawablePrivate))
{
    setDoubleSided(false);
}

}
}
