#ifndef CORE_RENDERPASS_H
#define CORE_RENDERPASS_H

#include <memory>

#include <utils/glm/vec3.hpp>

#include <core/stateset.h>

namespace simplex
{
namespace core
{

class RenderPass;
class ComputePass;

class Pass : public std::enable_shared_from_this<Pass>, public StateSet
{
public:
    ~Pass() override;

    virtual void run(const std::shared_ptr<graphics::RendererBase>&) = 0;

protected:
    Pass();
};

}
}

#endif // CORE_RENDERPASS_H