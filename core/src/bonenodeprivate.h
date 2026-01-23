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
    BoneNodePrivate(BoneNode&, const std::string&, uint32_t, const utils::Transform&);
    ~BoneNodePrivate() override;

    uint32_t& boneID();
    utils::Transform& offsetTransform();

private:
    uint32_t m_boneID;
    utils::Transform m_offsetTransform;
};

}
}

#endif // CORE_BONENODEPRIVATE_H
