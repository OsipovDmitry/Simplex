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
    auto currentRenderer = current();

    if (currentRenderer == shared_from_this())
        return true;

    currentRenderer->doneCurrent();

    RendererBasePrivate::current() = weak_from_this();
    return doMakeCurrent();
}

bool RendererBase::doneCurrent()
{
    auto currentRenderer = current();

    if (!currentRenderer)
        return true;

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