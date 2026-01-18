#include "renderpass.h"

namespace simplex
{
namespace core
{

RenderPass::~RenderPass() = default;

RenderPass::RenderPass(const std::weak_ptr<RenderPipeLine>& renderPipeLine)
    : StateSet()
    , m_renderPipeLine(renderPipeLine)
{
}

}
}
