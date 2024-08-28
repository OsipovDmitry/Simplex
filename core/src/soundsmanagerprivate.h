#ifndef CORE_SOUNDSMANAGERPRIVATE_H
#define CORE_SOUNDSMANAGERPRIVATE_H

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SoundsManagerPrivate
{
public:
    SoundsManagerPrivate(std::shared_ptr<audio::IRenderer>);
    ~SoundsManagerPrivate();

    std::shared_ptr<audio::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<audio::IBuffer>> &resources();

private:
    std::shared_ptr<audio::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<audio::IBuffer>> m_resources;

};

}
}

#endif // CORE_SOUNDSMANAGERPRIVATE_H
