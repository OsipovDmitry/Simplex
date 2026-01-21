#include "scenedata.h"
#include "drawableprivate.h"

namespace simplex
{
namespace core
{

DrawablePrivate::DrawablePrivate(const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material)
    : m_mesh(mesh)
    , m_material(material)
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

}
}
