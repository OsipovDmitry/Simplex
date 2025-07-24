#include <core/scene.h>

#include "scenedata.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

MeshHandler::MeshHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type id)
    : m_scene(scene)
    , m_mesh(mesh)
    , m_ID(id)
{}

MeshHandler::~MeshHandler()
{
    if (auto scene = m_scene.lock())
        if (auto mesh = m_mesh.lock())
            scene->m().removeMesh(mesh, m_ID);
}

std::weak_ptr<const Mesh>& MeshHandler::mesh()
{
    return m_mesh;
}

utils::IDsGenerator::value_type MeshHandler::ID() const
{
    return m_ID;
}

MaterialMapHandler::MaterialMapHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type id)
    : m_scene(scene)
    , m_materialMap(materialMap)
    , m_ID(id)
{}

MaterialMapHandler::~MaterialMapHandler()
{
    if (auto scene = m_scene.lock())
        if (auto materialMap = m_materialMap.lock())
            scene->m().removeMaterialMap(materialMap, m_ID);
}

std::weak_ptr<const MaterialMap>& MaterialMapHandler::materialMap()
{
    return m_materialMap;
}

utils::IDsGenerator::value_type& MaterialMapHandler::ID()
{
    return m_ID;
}

MaterialHandler::MaterialHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Material>& material, utils::IDsGenerator::value_type id)
    : m_scene(scene)
    , m_material(material)
    , m_ID(id)
{}

MaterialHandler::~MaterialHandler()
{
    if (auto scene = m_scene.lock())
        if (auto material = m_material.lock())
            scene->m().removeMaterial(material, m_ID);
}

std::weak_ptr<const Material>& MaterialHandler::material()
{
    return m_material;
}

utils::IDsGenerator::value_type MaterialHandler::ID() const
{
    return m_ID;
}

DrawableHandler::DrawableHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type id)
    : m_scene(scene)
    , m_drawable(drawable)
    , m_ID(id)
{
}

DrawableHandler::~DrawableHandler()
{
    if (auto scene = m_scene.lock())
        if (auto drawable = m_drawable.lock())
        scene->m().removeDrawable(drawable, m_ID);
}

std::weak_ptr<const Drawable>& DrawableHandler::drawable()
{
    return m_drawable;
}

utils::IDsGenerator::value_type DrawableHandler::ID() const
{
    return m_ID;
}

DrawDataHandler::DrawDataHandler(const std::weak_ptr<Scene>& scene, size_t id)
    : m_scene(scene)
    , m_ID(id)
{
}

DrawDataHandler::~DrawDataHandler()
{
    if (auto scene = m_scene.lock())
        scene->m().removeDrawData(m_ID);
}

size_t DrawDataHandler::ID() const
{
    return m_ID;
}

}
}
