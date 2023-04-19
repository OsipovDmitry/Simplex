#ifndef CORE_CAMERANODE_H
#define CORE_CAMERANODE_H

#include <utils/pimpl.h>
#include <utils/sortedobject.h>
#include <utils/glm/mat4x4.hpp>

#include <core/forwarddecl.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

class CameraNodePrivate;
class CORE_SHARED_EXPORT CameraNode : public Node, public utils::SortedObject
{
    PIMPL(CameraNode)
public:
    CameraNode(const std::string&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

    bool isRenderingEnabled() const;
    void setRenderingEnabled(bool);

    void setOrthoProjection();
    void setPerspectiveProjection(float fov);
    glm::mat4 projectionMatrix(float aspect, float zNear, float zFar) const;

protected:
    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;

};

}
}

#endif // CORE_CAMERANODE_H
