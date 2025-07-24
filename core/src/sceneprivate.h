#ifndef CORE_SCENEPRIVATE_H
#define CORE_SCENEPRIVATE_H

#include <string>
#include <memory>
#include <unordered_set>
#include <deque>

#include <utils/idgenerator.h>

#include <core/graphicsrendererbase.h>
#include <core/background.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

struct MaterialMapHandler;
struct MaterialHandler;
struct MeshHandler;

class ScenePrivate
{
public:
    ScenePrivate(Scene&, const std::string&);

    const std::string &name() const;

    std::shared_ptr<SceneRootNode> &sceneRootNode();
    std::shared_ptr<ListenerNode> &listenerNode();
    Background &background();

    utils::IDsGenerator::value_type addMesh(const std::shared_ptr<const Mesh>&);
    void removeMesh(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void onMeshChanged(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void addMeshData(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void removeMeshData(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addMaterialMap(const std::shared_ptr<const MaterialMap>&);
    void removeMaterialMap(const std::shared_ptr<const MaterialMap>&, utils::IDsGenerator::value_type);
    void onMaterialMapChanged(const std::shared_ptr<const MaterialMap>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addMaterial(const std::shared_ptr<const Material>&);
    void removeMaterial(const std::shared_ptr<const Material>&, utils::IDsGenerator::value_type);
    void onMaterialChanged(const std::shared_ptr<const Material>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addDrawable(const std::shared_ptr<const Drawable>&);
    void removeDrawable(const std::shared_ptr<const Drawable>&, utils::IDsGenerator::value_type);
    void onDrawableChanged(const std::shared_ptr<const Drawable>&, utils::IDsGenerator::value_type);

    std::shared_ptr<DrawDataHandler> addDrawData(const std::shared_ptr<const Drawable>&, const glm::mat4x4&);
    void removeDrawData(size_t);
    void onDrawDataChanged(const std::shared_ptr<const Drawable>&, const glm::mat4x4&, size_t);

    std::shared_ptr<graphics::DynamicBufferT<PositionDescription>>& positionsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<NormalDescription>>& normalsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<TexCoordsDescription>>& texCoordsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<BonesDescription>>& bonesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<TangentDescription>>& tangentsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<ColorDescription>>& colorsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<IndexDescription>>& indicesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MeshDescription>>& meshesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>> &texturesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>> &materialBuffer();
    std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>> &drawablesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription>> &drawDataBuffer();

private:
    Scene& d_;

    std::string m_name;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;
    Background m_background;

    std::shared_ptr<graphics::DynamicBufferT<PositionDescription>> m_positionsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<NormalDescription>> m_normalsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<TexCoordsDescription>> m_texCoordsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<BonesDescription>> m_bonesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<TangentDescription>> m_tangentsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<ColorDescription>> m_colorsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<IndexDescription>> m_indicesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MeshDescription>> m_meshesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>> m_materialMapsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>> m_materialsBuffer;    
    std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>> m_drawablesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription>> m_drawDataBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    std::unordered_map<std::shared_ptr<const Mesh>, std::weak_ptr<MeshHandler>> m_meshes;

    utils::IDsGenerator m_materialMapIDsGenerator;
    std::unordered_map<std::shared_ptr<const MaterialMap>, std::weak_ptr<MaterialMapHandler>> m_materialMaps;
    std::unordered_map<utils::IDsGenerator::value_type, graphics::PTextureHandle> m_materialMapsHandles;

    utils::IDsGenerator m_materialIDsGenerator;
    std::unordered_map<std::shared_ptr<const Material>, std::weak_ptr<MaterialHandler>> m_materials;

    utils::IDsGenerator m_drawableIDsGenerator;
    std::unordered_map<std::shared_ptr<const Drawable>, std::weak_ptr<DrawableHandler>> m_drawables;

    std::deque<std::weak_ptr<DrawDataHandler>> m_drawData;

};

}
}

#endif // CORE_SCENEPRIVATE_H
