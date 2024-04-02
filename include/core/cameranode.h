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

    void setOrthoProjection(float height);
    void setPerspectiveProjection(float fov);
    glm::mat4x4 calculateProjectionMatrix(float aspect, const utils::Range&) const;

    void resize(const glm::uvec2&);
    const glm::uvec2 &viewportSize() const;

    const utils::Range &cullPlanesLimits() const;
    void setCullPlanesLimits(const utils::Range&);

protected:
    bool canAttach(const std::shared_ptr<Node>&) override;
    bool canDetach(const std::shared_ptr<Node>&) override;


    void doDetach() override;
};

}
}

#endif // CORE_CAMERANODE_H
