#ifndef CORE_RENDERPASS_H
#define CORE_RENDERPASS_H

#include <core/coreglobal.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

class PassBasePrivate;
class CORE_SHARED_EXPORT PassBase : public StateSet
{
public:
    ~PassBase() override;

protected:
    PassBase(std::unique_ptr<PassBasePrivate>);
};

class RenderPassPrivate;
class CORE_SHARED_EXPORT RenderPass : public PassBase
{
public:
    ~RenderPass() override;

protected:
    RenderPass(std::unique_ptr<RenderPassPrivate>);
};

class ComputePassPrivate;
class CORE_SHARED_EXPORT ComputePass : public PassBase
{
public:
    ~ComputePass() override;

protected:
    ComputePass(std::unique_ptr<ComputePassPrivate>);
};

}
}

#endif // CORE_RENDERPASS_H