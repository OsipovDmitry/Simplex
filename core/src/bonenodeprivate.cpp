#include <core/bonenode.h>

#include "bonenodeprivate.h"

namespace simplex
{
namespace core
{

BoneNodePrivate::BoneNodePrivate(BoneNode& boneNode, const std::string& name, uint32_t boneID)
    : NodePrivate(boneNode, name)
    , m_boneID(boneID)
{
}

BoneNodePrivate::~BoneNodePrivate() = default;

uint32_t& BoneNodePrivate::boneID()
{
    return m_boneID;
}

}
}
