#include <core/scene.h>

#include "scenedata.h"
#include "drawableprivate.h"

namespace simplex
{
namespace core
{

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

std::set<std::shared_ptr<DrawableHandler>>& DrawablePrivate::handlers()
{
    return m_handlers;
}

void DrawablePrivate::onChanged()
{
    for (auto& handler : m_handlers)
        if (auto sceneData = handler->sceneData().lock())
            sceneData->onDrawableChanged(handler->ID(), m_mesh, m_material);
}

DrawablePrivate::DrawablePrivate(const std::shared_ptr<graphics::VAOMesh>& vao)
    : StateSetPrivate()
    , m_VAOMesh(vao)
    , m_isDoubleSided(true)
{}

std::shared_ptr<graphics::VAOMesh>& DrawablePrivate::vertexArray()
{
    return m_VAOMesh;
}

bool& DrawablePrivate::isDoubleSided()
{
    return m_isDoubleSided;
}

}
}
