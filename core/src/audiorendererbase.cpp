#include "audiorendererbaseprivate.h"
#include "audiorendererbase.h"

namespace simplex
{
namespace core
{
namespace audio
{

RendererBase::RendererBase(const std::string &name)
    : m_(std::make_unique<RendererBasePrivate>(name))
{
}

RendererBase::~RendererBase() = default;

const std::string& RendererBase::name() const
{
    return m_->name();
}

bool RendererBase::makeCurrent()
{
    RendererBasePrivate::current() = weak_from_this();

    return doMakeCurrent();
}

bool RendererBase::doneCurrent()
{
    RendererBasePrivate::current().reset();

    return doDoneCurrent();
}

std::shared_ptr<RendererBase> RendererBase::current()
{
    auto currentWeak = RendererBasePrivate::current();

    return currentWeak.expired() ? nullptr : currentWeak.lock();
}


}
}
}