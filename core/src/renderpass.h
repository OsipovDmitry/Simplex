#ifndef CORE_RENDERPASS_H
#define CORE_RENDERPASS_H

#include <memory>

#include <core/stateset.h>

namespace simplex
{
namespace core
{

class GeometryBuffer;
class SceneData;

class RenderPass : public StateSet, public std::enable_shared_from_this<RenderPass>
{
public:
    ~RenderPass() override;

    virtual void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) = 0;

protected:
    RenderPass();
};

}
}

#endif // CORE_RENDERPASS_H