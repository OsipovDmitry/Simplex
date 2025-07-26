#ifndef CORE_RENDERPASSPRIVATE_H
#define CORE_RENDERPASSPRIVATE_H

#include "statesetprivate.h"

namespace simplex
{
namespace core
{

class PassBasePrivate : public StateSetPrivate
{
public:
    PassBasePrivate();
    ~PassBasePrivate() override;

protected:
    std::shared_ptr<graphics::IProgram> m_program;
};

class RenderPassPrivate : public PassBasePrivate
{
public:
    RenderPassPrivate();
    ~RenderPassPrivate() override;

protected:

};

class ComputePassPrivate : public PassBasePrivate
{
public:
    ComputePassPrivate();
    ~ComputePassPrivate() override;

protected:

};

}
}

#endif // CORE_RENDERPASSPRIVATE_H