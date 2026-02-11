#ifndef CORE_AUDIOENGINE_H
#define CORE_AUDIOENGINE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class AudioEnginePrivate;
class CORE_SHARED_EXPORT AudioEngine : public IEngine
{
    NONCOPYBLE(AudioEngine)
    PRIVATE_IMPL(AudioEngine)

public:
    AudioEngine(const std::string&, std::shared_ptr<audio::RendererBase>);
    ~AudioEngine() override;

    const std::string& name() const override;

    std::shared_ptr<IRenderer> renderer() override final;
    std::shared_ptr<const IRenderer> renderer() const override final;

    std::shared_ptr<audio::RendererBase> audioRenderer();
    std::shared_ptr<const audio::RendererBase> audioRenderer() const;

    std::shared_ptr<SoundsLoader> soundsLoader();
    std::shared_ptr<const SoundsLoader> soundsLoader() const;

    void update(const std::shared_ptr<Scene>& scene, uint64_t time, uint32_t dt);

private:
    std::unique_ptr<AudioEnginePrivate> m_;
};

}
}

#endif // CORE_AUDIOENGINE_H
