#include <core/soundnode.h>

#include "soundnodeprivate.h"

namespace simplex
{
namespace core
{

SoundNodePrivate::SoundNodePrivate(SoundNode &soundNode, const std::string &name)
    : NodePrivate(soundNode, name)
{
}

SoundNodePrivate::~SoundNodePrivate() = default;

void SoundNodePrivate::doAttachToScene()
{

}

void SoundNodePrivate::doDetachFromScene()
{

}

std::shared_ptr<audio::ISource> &SoundNodePrivate::source()
{
    return m_source;
}

}
}
