#include <utils/fileextension.h>
#include <utils/sound.h>

#include <core/audiorendererbase.h>
#include <core/soundsloader.h>

#include "soundsloaderprivate.h"

namespace simplex
{
namespace core
{

class WAVSoundsLoader : public LoaderBase<audio::IBuffer>
{
public:
    WAVSoundsLoader(const std::string& name, const std::shared_ptr<audio::RendererBase>& audioRenderer)
        : LoaderBase<audio::IBuffer>(name)
        , m_audioRenderer(audioRenderer)
    {}

    ~WAVSoundsLoader() override = default;

    std::shared_ptr<audio::IBuffer> loadResource(const std::filesystem::path& absoluteFileName) const override
    {
        std::shared_ptr<audio::IBuffer> buffer;
        if (auto sound = utils::Sound::loadFromFile(absoluteFileName))
        {
            m_audioRenderer->makeCurrent();
            buffer = m_audioRenderer->createBuffer(sound);
        }
        return buffer;
    }

private:
    std::shared_ptr<audio::RendererBase> m_audioRenderer;
};

SoundsLoader::SoundsLoader(const std::string& name, const std::shared_ptr<audio::RendererBase>& renderer)
    : ResourceLoaderBase<audio::IBuffer>(name)
    , m_(std::make_unique<SoundsLoaderPrivate>(renderer))
{
    registerLoader(std::make_shared<WAVSoundsLoader>(name + "WAVLoader", renderer));
}

SoundsLoader::~SoundsLoader() = default;

}
}
