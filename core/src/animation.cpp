#include <algorithm>

#include <utils/logger.h>

#include <core/animation.h>
#include <core/applicationbase.h>
#include <core/graphicsengine.h>
#include <core/igraphicsrenderer.h>

#include "animationprivate.h"

namespace simplex
{
namespace core
{

Animation::Animation(const std::string &name)
    : m_(std::make_unique<AnimationPrivate>(name))
{
}

Animation::~Animation() = default;

const std::string &Animation::name() const
{
    return m_->name();
}

const std::vector<std::shared_ptr<Animation::Channel>> &Animation::channels() const
{
    return m_->channels();
}

std::shared_ptr<Animation::Channel> Animation::addChannel()
{
    auto channel = std::make_shared<Channel>();
    m_->channels().push_back(channel);
    return channel;
}

void Animation::removeChannel(const std::shared_ptr<Channel> &value)
{
    if (auto it = std::find(m_->channels().begin(), m_->channels().end(), value); it != m_->channels().end())
        m_->channels().erase(it);
}

Skeleton::Skeleton()
    : m_(std::make_unique<SkeletonPrivate>())
{
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto graphicsEngine = app->findEngine<GraphicsEngine>("");
    if (!graphicsEngine)
        LOG_CRITICAL << "Graphics engine can't be nullptr";

    auto graphicsRenderer = graphicsEngine->graphicsRenderer();
    m_->bonesBuffer() = graphicsRenderer->createBufferRange(graphicsRenderer->createBuffer(), 0u);
}

Skeleton::~Skeleton() = default;

const std::vector<std::shared_ptr<Skeleton::Bone>> &Skeleton::bones() const
{
    return m_->bones();
}

void Skeleton::setBones(const std::vector<std::shared_ptr<Bone>> &value)
{
    m_->bones() = value;

    const auto bufferTotalSize = m_->bones().size() * sizeof(decltype(Skeleton::Bone::inverseBindMatrix));
    if (auto buffer = m_->bonesBuffer()->buffer(); buffer->size() != bufferTotalSize)
        buffer->resize(bufferTotalSize, nullptr);
}

graphics::PConstBufferRange Skeleton::bonesBuffer() const
{
    return m_->bonesBuffer();
}

}
}
