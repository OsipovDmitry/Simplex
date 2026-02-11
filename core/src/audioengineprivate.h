#ifndef CORE_AUDIOENGINEPRIVATE_H
#define CORE_AUDIOENGINEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class AudioEnginePrivate
{
public:
    AudioEnginePrivate(const std::string&);

    std::string &name();
    std::shared_ptr<audio::RendererBase> &renderer();
    std::shared_ptr<SoundsLoader> &soundsLoader();

private:
    std::string m_name;
    std::shared_ptr<audio::RendererBase> m_renderer;
    std::shared_ptr<SoundsLoader> m_soundsLoader;
};

}
}

#endif // CORE_AUDIOENGINEPRIVATE_H
