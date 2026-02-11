#ifndef CORE_SOUNDSLOADERPRIVATE_H
#define CORE_SOUNDSLOADERPRIVATE_H

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <string>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SoundsLoaderPrivate
{
public:
    SoundsLoaderPrivate(const std::shared_ptr<audio::RendererBase>&);
    ~SoundsLoaderPrivate();

    std::shared_ptr<audio::RendererBase> &renderer();

private:
    std::shared_ptr<audio::RendererBase> m_renderer;

};

}
}

#endif // CORE_SOUNDSLOADERPRIVATE_H
