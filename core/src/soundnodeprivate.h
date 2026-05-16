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

    void onDetachFromScene(const std::shared_ptr<Scene>&) override;

    std::shared_ptr<audio::ISource>& source();
    SoundState& state();

private:
    std::shared_ptr<audio::ISource> m_source;
    SoundState m_state;
};

} // namespace core
} // namespace simplex

#endif // CORE_SOUNDNODEPRIVATE_H
