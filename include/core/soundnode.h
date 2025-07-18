#ifndef CORE_SOUNDNODE_H
#define CORE_SOUNDNODE_H

#include <utils/pimpl.h>
#include <utils/enumclass.h>

#include <core/forwarddecl.h>
#include <core/node.h>


namespace simplex
{
namespace core
{

ENUMCLASS(SoundState, uint16_t,
          Play, Pause, Stop)

class SoundNodePrivate;
class CORE_SHARED_EXPORT SoundNode : public Node
{
    PRIVATE_IMPL(SoundNode)
public:
    SoundNode(const std::string&);
    ~SoundNode() override;

    std::shared_ptr<SoundNode> asSoundNode() override;
    std::shared_ptr<const SoundNode> asSoundNode() const override;

    std::shared_ptr<core::audio::ISource> source();
    std::shared_ptr<const core::audio::ISource> source() const;

    SoundState state() const;
    void setState(SoundState);

};

}
}

#endif // CORE_SOUNDNODE_H
