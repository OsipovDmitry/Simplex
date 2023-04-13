#ifndef CORE_CAMERANODE_H
#define CORE_CAMERANODE_H

#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/node.h>
#include <core/sortedobject.h>

namespace simplex
{
namespace core
{

class CameraNodePrivate;
class CORE_SHARED_EXPORT CameraNode : public Node, public SortedObject
{
    PIMPL(CameraNode)
public:
    CameraNode(const std::string&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

protected:
    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;

};

}
}

#endif // CORE_CAMERANODE_H
