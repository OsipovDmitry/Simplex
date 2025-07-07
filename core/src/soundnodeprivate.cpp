#include <utils/logger.h>

#include <core/audiorendererbase.h>
#include <core/soundnode.h>

#include "soundnodeprivate.h"

namespace simplex
{
namespace core
{

SoundNodePrivate::SoundNodePrivate(SoundNode &soundNode, const std::string &name)
    : NodePrivate(soundNode, name)
    , m_state(SoundState::Stop)
{
}

SoundNodePrivate::~SoundNodePrivate() = default;

void SoundNodePrivate::doDetachFromScene()
{
    auto audioRenderer = audio::RendererBase::current();
    if (!audioRenderer)
        LOG_CRITICAL << "Audio renderer can't be nullptr";

    audioRenderer->pauseSource(m_source);
}

std::shared_ptr<audio::ISource> &SoundNodePrivate::source()
{
    return m_source;
}

SoundState &SoundNodePrivate::state()
{
    return m_state;
}

}
}
