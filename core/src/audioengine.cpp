#include <utils/logger.h>
#include <utils/transform.h>

#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/nodecollector.h>
#include <core/iaudiorenderer.h>
#include <core/audioengine.h>
#include <core/listenernode.h>
#include <core/soundnode.h>
#include <core/soundsmanager.h>

#include "audioengineprivate.h"

namespace simplex
{
namespace core
{

AudioEngine::AudioEngine(const std::string &name, std::shared_ptr<audio::IRenderer> renderer)
    : m_(std::make_unique<AudioEnginePrivate>(name))
{
    if (!renderer)
        LOG_CRITICAL << "Audio renderer can't be nullptr";

    m_->renderer() = renderer;
    m_->soundsManager() = std::make_shared<SoundsManager>(renderer);

    LOG_INFO << "Engine \"" << AudioEngine::name() << "\" has been created";
}

AudioEngine::~AudioEngine()
{
    LOG_INFO << "Engine \"" << AudioEngine::name() << "\" has been destroyed";
}

const std::string &AudioEngine::name() const
{
    return m_->name();
}

void AudioEngine::update(const std::shared_ptr<IRenderWidget> &/*renderWidget*/,
                         const std::shared_ptr<Scene> &scene,
                         uint64_t /*time*/,
                         uint32_t /*dt*/,
                         debug::SceneInformation&)
{
    static const auto s_localDirection = glm::vec3(0.f, 0.f, -1.f);

    auto renderer = m_->renderer();
    renderer->makeCurrent();

    utils::Transform listenerTranform = scene->listenerNode()->globalTransform();

    auto listener = renderer->listener();
    listener->setPosition(listenerTranform.translation);
    listener->setOrientation(listenerTranform.rotation);

    NodeCollector<SoundNode> soundNodeCollector;
    scene->sceneRootNode()->acceptDown(soundNodeCollector);

    for (const auto &soundNode : soundNodeCollector.nodes())
    {
        auto source = soundNode->source();

        const auto &transform = soundNode->globalTransform();
        source->setPosition(transform.translation);
        source->setDirection(transform.rotation * s_localDirection);

        switch (soundNode->state())
        {
        case SoundState::Stop:
        {
            renderer->stopSource(source);
            break;
        }
        case SoundState::Pause:
        {
            renderer->pauseSource(source);
            break;
        }
        case SoundState::Play:
        {
            if (renderer->sourceState(source) != audio::SourceState::Playing)
                renderer->playSource(source);
            break;
        }
        default:
            break;
        }

    }
}

std::shared_ptr<audio::IRenderer> AudioEngine::audioRenderer()
{
    return m_->renderer();
}

std::shared_ptr<const audio::IRenderer> AudioEngine::audioRenderer() const
{
    return const_cast<AudioEngine*>(this)->audioRenderer();
}

std::shared_ptr<SoundsManager> AudioEngine::soundsManager()
{
    return m_->soundsManager();
}

std::shared_ptr<const SoundsManager> AudioEngine::soundsManager() const
{
    return const_cast<AudioEngine*>(this)->soundsManager();
}

}
}
