#include "soundsmanagerprivate.h"

namespace simplex
{
namespace core
{

SoundsManagerPrivate::SoundsManagerPrivate(const std::shared_ptr<audio::RendererBase> &renderer)
    : m_renderer(renderer)
{
}

SoundsManagerPrivate::~SoundsManagerPrivate() = default;

std::shared_ptr<audio::RendererBase> &SoundsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<audio::IBuffer> > &SoundsManagerPrivate::resources()
{
    return m_resources;
}

}
}
