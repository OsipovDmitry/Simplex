#include "renderpassprivate.h"

namespace simplex
{
namespace core
{

PassBasePrivate::PassBasePrivate()
    : StateSetPrivate()
{
}

PassBasePrivate::~PassBasePrivate() = default;

RenderPassPrivate::RenderPassPrivate()
    : PassBasePrivate()
{
}

RenderPassPrivate::~RenderPassPrivate() = default;


ComputePassPrivate::ComputePassPrivate()
    : PassBasePrivate()
{
}

ComputePassPrivate::~ComputePassPrivate() = default;
}
}
