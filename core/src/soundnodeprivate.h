#ifndef CORE_SOUNDNODEPRIVATE_H
#define CORE_SOUNDNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class SoundNodePrivate : public NodePrivate
{
public:
    SoundNodePrivate(SoundNode&, const std::string&);
    ~SoundNodePrivate() override;

    void doAttachToScene() override;
    void doDetachFromScene() override;

    std::shared_ptr<audio::ISource> &source();

private:
    std::shared_ptr<audio::ISource> m_source;
};

}
}

#endif // CORE_SOUNDNODEPRIVATE_H
