#ifndef CORE_GEOMETRYRENDERINFO_H
#define CORE_GEOMETRYRENDERINFO_H

#include <core/forwarddecl.h>
#include <core/stateset.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

using PCameraBuffer = std::shared_ptr<graphics::StructBuffer<CameraDescription>>;

using PClusterNodesBuffer = std::shared_ptr<graphics::VectorBuffer<ClusterNodeDescription>>;
using PLightNodesBuffer = std::shared_ptr<graphics::VectorBuffer<LightNodeDescription>>;

using PSceneInfoBuffer = std::shared_ptr<graphics::StructBuffer<SceneInfoDescription>>;
using PSceneInfoConstBuffer = std::shared_ptr<const graphics::StructBuffer<SceneInfoDescription>>;

class CameraRenderInfo : public StateSet
{
public:
    CameraRenderInfo();
    ~CameraRenderInfo() override;

    void setFrustum(const utils::Frustum&);
    void resetSceneInfo(const glm::uvec3& clusterSize, uint32_t drawDataCount, uint32_t lightsCount, uint32_t OITNodesMaxCount);

    PSceneInfoConstBuffer sceneInfoBuffer() const;
    utils::Range sceneInfoZRange() const;

    //tmp
    PClusterNodesBuffer clusterNodesBuffer() const { return m_clusterNodesBuffer; }

private:
    PCameraBuffer m_cameraBuffer;
    PClusterNodesBuffer m_clusterNodesBuffer;
    PLightNodesBuffer m_lightNodesBuffer;
    PSceneInfoBuffer m_sceneInfoBuffer;
};

}
}

#endif // CORE_GEOMETRYRENDERINFO_H
