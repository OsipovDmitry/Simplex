#ifndef CORE_AUDIOENGINE_H
#define CORE_AUDIOENGINE_H

#include <memory>
#include <string>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class AudioEnginePrivate;
class CORE_SHARED_EXPORT AudioEngine : public std::enable_shared_from_this<AudioEngine>, public IEngine
{
    NONCOPYBLE(AudioEngine)

public:
    AudioEngine(const std::string&, std::shared_ptr<audio::IRenderer>);
    ~AudioEngine() override;

    const std::string &name() const override;
    void update(const std::shared_ptr<IRenderWidget>&,
                const std::shared_ptr<Scene>&,
                uint64_t time,
                uint32_t dt,
                debug::SceneInformation&) override;

    std::shared_ptr<audio::IRenderer> audioRenderer();
    std::shared_ptr<const audio::IRenderer> audioRenderer() const;

    std::shared_ptr<SoundsManager> soundsManager();
    std::shared_ptr<const SoundsManager> soundsManager() const;

private:
    std::unique_ptr<AudioEnginePrivate> m_;

    std::shared_ptr<audio::ISource> source = nullptr;

};

}
}

#endif // CORE_AUDIOENGINE_H
