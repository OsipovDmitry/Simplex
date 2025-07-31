#include "audioengineprivate.h"

namespace simplex
{
namespace core
{

AudioEnginePrivate::AudioEnginePrivate(const std::string &name)
    : m_name(name)
{
}

std::string& AudioEnginePrivate::name()
{
    return m_name;
}

std::shared_ptr<audio::RendererBase> &AudioEnginePrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<SoundsManager> &AudioEnginePrivate::soundsManager()
{
    return m_soundsManager;
}

std::shared_ptr<Scene>& AudioEnginePrivate::scene()
{
    return m_scene;
}

}
}
