#ifndef CORE_RENDERPASS_H
#define CORE_RENDERPASS_H

#include <memory>

#include <utils/glm/vec3.hpp>

#include <core/stateset.h>

namespace simplex
{
namespace core
{

class GeometryRenderInfo;

class Pass : public std::enable_shared_from_this<Pass>, public StateSet
{
public:
    ~Pass() override;

    virtual void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&) = 0;

protected:
    Pass();
};

}
}

#endif // CORE_RENDERPASS_H