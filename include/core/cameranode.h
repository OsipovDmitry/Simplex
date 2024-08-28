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
    CameraNode(const std::string&, const std::shared_ptr<graphics::IFrameBuffer>&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

    bool isRenderingEnabled() const;
    void setRenderingEnabled(bool);

    std::shared_ptr<graphics::IFrameBuffer> frameBuffer();
    std::shared_ptr<const graphics::IFrameBuffer> frameBuffer() const;
    void setFrameBuffer(const std::shared_ptr<graphics::IFrameBuffer>&);

    const utils::ClipSpace &clipSpace() const;
    void setOrthoClipSpace(float height);
    void setPerspectiveClipSpace(float fovY);

    const utils::Range &cullPlanesLimits() const;
    void setCullPlanesLimits(const utils::Range&);

    SSAO &ssao();
    const SSAO &ssao() const;
};

}
}

#endif // CORE_CAMERANODE_H
