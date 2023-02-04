#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <memory>

#include <core/forwarddecl.h>

class AudioEngine : public IEngine
{
public:
    AudioEngine(std::shared_ptr<IAudioRenderer>);

    void update() override;
};

#endif // AUDIOENGINE_H
