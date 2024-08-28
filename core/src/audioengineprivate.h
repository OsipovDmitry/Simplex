#ifndef CORE_AUDIOENGINEPRIVATE_H
#define CORE_AUDIOENGINEPRIVATE_H

#include <string>
#include <memory>

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

    std::shared_ptr<audio::IRenderer> &renderer();
    std::shared_ptr<SoundsManager> &soundsManager();

private:
    std::string m_name;

    std::shared_ptr<audio::IRenderer> m_renderer;
    std::shared_ptr<SoundsManager> m_soundsManager;
};

}
}

#endif // CORE_AUDIOENGINEPRIVATE_H
