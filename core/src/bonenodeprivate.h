#ifndef CORE_BONENODEPRIVATE_H
#define CORE_BONENODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class BoneNodePrivate : public NodePrivate
{
public:
    BoneNodePrivate(BoneNode&, const std::string&, uint32_t);
    ~BoneNodePrivate() override;

    uint32_t& boneID();

private:
    uint32_t m_boneID;
};

}
}

#endif // CORE_BONENODEPRIVATE_H
