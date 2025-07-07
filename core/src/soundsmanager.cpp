#include <utils/fileextension.h>
#include <utils/logger.h>
#include <utils/sound.h>

#include <core/audiorendererbase.h>
#include <core/soundsmanager.h>

#include "soundsmanagerprivate.h"

namespace simplex
{
namespace core
{

SoundsManager::SoundsManager(const std::shared_ptr<audio::RendererBase>& renderer)
    : m_(std::make_unique<SoundsManagerPrivate>(renderer))
{
}

SoundsManager::~SoundsManager() = default;

std::shared_ptr<audio::IBuffer> SoundsManager::loadOrGetSound(const std::filesystem::path &filename, const std::string &key)
{
    const auto absoluteFilename = std::filesystem::absolute(filename);

    auto name = key;
    if (name == "autogen")
    {
        name = absoluteFilename.string();
    }

    if (auto it = m_->resources().find(name); it != m_->resources().end())
    {
        return it->second;
    }

    std::shared_ptr<audio::IBuffer> buffer;

    auto sound = utils::Sound::loadFromFile(absoluteFilename);
    if (sound)
        buffer = m_->renderer()->createBuffer(sound);
    else
        LOG_ERROR << "Can't open sound file " << absoluteFilename;


    if (!buffer)
        LOG_CRITICAL << "Failed to load sound " << absoluteFilename;

    m_->resources()[name] = buffer;
    return buffer;
}

}
}
