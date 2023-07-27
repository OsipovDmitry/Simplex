#ifndef CORE_CAMERANODE_H
#define CORE_CAMERANODE_H

#include <utils/pimpl.h>
#include <utils/sortedobject.h>
#include <utils/glm/mat4x4.hpp>

#include <core/forwarddecl.h>
#include <core/node.h>
#include <core/rendersurface.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT GSurface : public RenderSurface
{
public:
    ~GSurface() override;

    std::shared_ptr<graphics::ITexture> colorBuffer0();
    std::shared_ptr<graphics::ITexture> colorBuffer1();
    std::shared_ptr<graphics::ITexture> depthBuffer();

protected:
    GSurface(std::shared_ptr<graphics::IRenderer>);

    void doResize(const glm::uvec2&) override;

    friend class CameraNode;
};

class CameraNodePrivate;
class CORE_SHARED_EXPORT CameraNode : public Node, public utils::SortedObject
{
    PIMPL(CameraNode)
public:
    CameraNode(std::shared_ptr<RenderSurface>, const std::string&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

    void resizeRenderSurfaces(const glm::uvec2&);

    bool isRenderingEnabled() const;
    void setRenderingEnabled(bool);

    void setOrthoProjection();
    void setPerspectiveProjection(float fov);
    glm::mat4 projectionMatrix(float aspect, float zNear, float zFar) const;

    std::shared_ptr<RenderSurface> renderSurface();
    std::shared_ptr<GSurface> gSurface();
    void setRenderSurface(std::shared_ptr<RenderSurface>);

protected:
    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;

};

}
}

#endif // CORE_CAMERANODE_H
