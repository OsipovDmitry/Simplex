#include <core/scene.h>

#include "scenedata.h"
#include "drawableprivate.h"

namespace simplex
{
namespace core
{

DrawablePrivate::DrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : StateSetPrivate()
    , m_vertexArray(vao)
    , m_isDoubleSided(true)
{}

DrawablePrivate::DrawablePrivate(const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material)
    : StateSetPrivate()
    , m_mesh(mesh)
    , m_material(material)
    , m_isDoubleSided(true)
{}

DrawablePrivate::~DrawablePrivate() = default;

std::shared_ptr<const Mesh>& DrawablePrivate::mesh()
{
    return m_mesh;
}

std::shared_ptr<const Material>& DrawablePrivate::material()
{
    return m_material;
}

std::shared_ptr<graphics::IVertexArray> &DrawablePrivate::vertexArray()
{
    return m_vertexArray;
}

bool &DrawablePrivate::isDoubleSided()
{
    return m_isDoubleSided;
}

std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<DrawableHandler>>& DrawablePrivate::handles()
{
    return m_handles;
}

void DrawablePrivate::onChanged()
{
    for (auto& handle : m_handles)
        handle.first->onDrawableChanged(handle.second->drawable().lock(), handle.second->ID());
}

}
}
