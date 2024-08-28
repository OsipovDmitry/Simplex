#include "soundsmanagerprivate.h"

namespace simplex
{
namespace core
{

SoundsManagerPrivate::SoundsManagerPrivate(std::shared_ptr<audio::IRenderer> renderer)
    : m_renderer(renderer)
{
}

SoundsManagerPrivate::~SoundsManagerPrivate() = default;

std::shared_ptr<audio::IRenderer> &SoundsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<audio::IBuffer> > &SoundsManagerPrivate::resources()
{
    return m_resources;
}

}
}
