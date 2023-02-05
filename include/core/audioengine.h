#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <memory>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT AudioEngine : public IEngine
{
    NONCOPYBLE(AudioEngine)
public:
    AudioEngine(std::shared_ptr<IAudioRenderer>);

    void update() override;
};

}
}

#endif // AUDIOENGINE_H
