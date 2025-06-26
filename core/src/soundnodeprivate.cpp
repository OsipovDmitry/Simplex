#include <utils/logger.h>

#include <core/applicationbase.h>
#include <core/audioengine.h>
#include <core/iaudiorenderer.h>
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
    auto app = ApplicationBase::currentApplication();
    if (!app)
        LOG_CRITICAL << "Application can't be nullptr";

    auto audioEngine = app->findEngine<AudioEngine>("");
    if (!audioEngine)
        LOG_CRITICAL << "Audio engine can't be nullptr";

    auto audioRenderer = audioEngine->audioRenderer();
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
