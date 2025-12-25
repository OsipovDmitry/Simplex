#ifndef CORE_CAMERANODE_H
#define CORE_CAMERANODE_H

#include <utils/sortedobject.h>
#include <utils/glm/mat4x4.hpp>

#include <core/node.h>

namespace simplex
{
namespace core
{

class CameraNodePrivate;
class CORE_SHARED_EXPORT CameraNode : public Node, public utils::SortedObject
{
    PRIVATE_IMPL(CameraNode)
public:
    CameraNode(const std::string&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

    bool isRenderingEnabled() const;
    void setRenderingEnabled(bool);

    bool isDefaultFramebufferUsed() const;
    void useDefaultFramebuffer();
    void useSeparateFramebuffer(const glm::uvec2&);

    const utils::ClipSpace &clipSpace() const;
    void setOrthoClipSpace(float height);
    void setPerspectiveClipSpace(float fovY);

    const utils::Range &cullPlanesLimits() const;
    void setCullPlanesLimits(const utils::Range&);

    const glm::uvec3& clusterMaxSize() const;
    void setClusterMaxSize(const glm::uvec3&);

    SSAO &ssao();
    const SSAO &ssao() const;
};

}
}

#endif // CORE_CAMERANODE_H
