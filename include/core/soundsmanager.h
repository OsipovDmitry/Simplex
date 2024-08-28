#ifndef CORE_SOUNDSMANAGER_H
#define CORE_SOUNDSMANAGER_H

#include <memory>
#include <filesystem>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SoundsManagerPrivate;
class CORE_SHARED_EXPORT SoundsManager
{
    NONCOPYBLE(SoundsManager)
public:
    SoundsManager(std::shared_ptr<audio::IRenderer>);
    ~SoundsManager();

    std::shared_ptr<audio::IBuffer> loadOrGetSound(const std::filesystem::path&, const std::string& = "autogen");


private:
    std::unique_ptr<SoundsManagerPrivate> m_;
};

}
}

#endif // CORE_SOUNDSMANAGER_H
