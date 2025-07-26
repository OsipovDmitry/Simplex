#include <core/renderpass.h>

#include "renderpassprivate.h"

namespace simplex
{
namespace core
{

PassBase::~PassBase() = default;

PassBase::PassBase(std::unique_ptr<PassBasePrivate> passBasePrivate)
    : StateSet(std::move(passBasePrivate))
{
}

RenderPass::~RenderPass() = default;


RenderPass::RenderPass(std::unique_ptr<RenderPassPrivate> renderPassPrivate)
    : PassBase(std::move(renderPassPrivate))
{
}

ComputePass::~ComputePass() = default;


ComputePass::ComputePass(std::unique_ptr<ComputePassPrivate> computePassPrivate)
    : PassBase(std::move(computePassPrivate))
{
}


}
}
