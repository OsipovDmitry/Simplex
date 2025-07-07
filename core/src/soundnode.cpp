#include <utils/logger.h>

#include <core/scene.h>
#include <core/audiorendererbase.h>
#include <core/soundnode.h>

#include "soundnodeprivate.h"

namespace simplex
{
namespace core
{

SoundNode::SoundNode(const std::string &name)
    : Node(std::make_unique<SoundNodePrivate>(*this, name))
{
    auto audioRenderer = audio::RendererBase::current();
    if (!audioRenderer)
        LOG_CRITICAL << "Audio renderer can't be nullptr";

    auto source = audioRenderer->createSource();
    if (!source)
        LOG_CRITICAL << "Audio source can't be nullptr";

    m().source() = source;
}

SoundNode::~SoundNode() = default;

std::shared_ptr<SoundNode> SoundNode::asSoundNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<SoundNode>(wp.lock());
}

std::shared_ptr<const SoundNode> SoundNode::asSoundNode() const
{
    return const_cast<SoundNode*>(this)->asSoundNode();
}

std::shared_ptr<audio::ISource> SoundNode::source()
{
    return m().source();
}

std::shared_ptr<const audio::ISource> SoundNode::source() const
{
    return const_cast<SoundNode*>(this)->source();
}

SoundState SoundNode::state() const
{
    return m().state();
}

void SoundNode::setState(SoundState value)
{
    m().state() = value;
}

}
}
