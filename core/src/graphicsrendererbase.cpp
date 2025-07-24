#include <utils/logger.h>

#include <core/igraphicswidget.h>

#include "graphicsrendererbaseprivate.h"
#include "graphicsrendererbase.h"

namespace simplex
{
namespace core
{
namespace graphics
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

    bool result = doMakeCurrent();
    RendererBasePrivate::current() = weak_from_this();

    return result;
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
    auto &currentWeak = RendererBasePrivate::current();

    return currentWeak.expired() ? nullptr : currentWeak.lock();
}

bool RendererBase::areShared(const std::shared_ptr<const RendererBase>& renderer1, const std::shared_ptr<const RendererBase>& renderer2)
{
    if (!renderer1 || !renderer2)
        return false;

    auto widget1 = renderer1->widget();
    auto widget2 = renderer1->widget();
    if (!widget1 || !widget2)
        return false;

    return widget1->shareGroup() == widget2->shareGroup();
}

}
}
}