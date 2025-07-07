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
    SoundsManagerPrivate(const std::shared_ptr<audio::RendererBase>&);
    ~SoundsManagerPrivate();

    std::shared_ptr<audio::RendererBase> &renderer();
    std::unordered_map<std::string, std::shared_ptr<audio::IBuffer>> &resources();

private:
    std::shared_ptr<audio::RendererBase> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<audio::IBuffer>> m_resources;

};

}
}

#endif // CORE_SOUNDSMANAGERPRIVATE_H
