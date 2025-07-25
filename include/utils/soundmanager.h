#ifndef UTILS_SOUNDMANAGER_H
#define UTILS_SOUNDMANAGER_H

#include <memory>
#include <filesystem>
#include <unordered_map>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/singleton.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT SoundManager final
{
    NONCOPYBLE(SoundManager)
    SINGLETON(SoundManager)
public:
    ~SoundManager();


    std::shared_ptr<Sound> loadOrGet(const std::filesystem::path&);

private:
    SoundManager();

    std::unordered_map<std::filesystem::path, std::shared_ptr<Sound>> m_sounds;
};

}
}

#endif // UTILS_SOUNDMANAGER_H
