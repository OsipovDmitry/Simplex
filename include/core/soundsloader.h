#ifndef CORE_SOUNDSLOADER_H
#define CORE_SOUNDSLOADER_H

#include <core/forwarddecl.h>
#include <core/resourceloader.h>

namespace simplex
{
namespace core
{

class SoundsLoaderPrivate;
class CORE_SHARED_EXPORT SoundsLoader : public ResourceLoaderBase<audio::IBuffer>
{
    NONCOPYBLE(SoundsLoader)
public:
    SoundsLoader(const std::string&, const std::shared_ptr<audio::RendererBase>&);
    ~SoundsLoader();

private:
    std::unique_ptr<SoundsLoaderPrivate> m_;
};

}
}

#endif // CORE_SOUNDSLOADER_H
