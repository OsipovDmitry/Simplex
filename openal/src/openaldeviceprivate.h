#ifndef OPENALDEVICEPRIVATE_H
#define OPENALDEVICEPRIVATE_H

#include <unordered_set>
#include <string>
#include <memory>
#include <alc.h>

#include <openal/openaldevice.h>

namespace simplex
{
namespace openal
{

inline bool operator ==(const std::weak_ptr<OpenALDevice>& l, const std::weak_ptr<OpenALDevice>& r)
{
    if (l.expired() || r.expired())
        return false;

    return l.lock() == r.lock();
}

struct OpenALDeviceHash
{
public:
    size_t operator ()(const std::weak_ptr<OpenALDevice>& value) const
    {
        return value.expired() ? 0u : std::hash<std::string>()(value.lock()->name());
    }
};

class OpenAL_1_1_Renderer;

class OpenALDevicePrivate
{
public:
    OpenALDevicePrivate(const std::string&);

    std::string &name();
    ALCdevice *&device();
    std::shared_ptr<OpenAL_1_1_Renderer> &renderer();

    static std::unordered_set<std::weak_ptr<OpenALDevice>, OpenALDeviceHash> &instances();

private:
    std::string m_name;
    ALCdevice *m_device;
    std::shared_ptr<OpenAL_1_1_Renderer> m_renderer;

    static std::unordered_set<std::weak_ptr<OpenALDevice>, OpenALDeviceHash> m_instances;
};

}
}

#endif // OPENALDEVICEPRIVATE_H
