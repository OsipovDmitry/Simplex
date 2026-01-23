#include <core/bonenode.h>

#include "bonenodeprivate.h"

namespace simplex
{
namespace core
{

BoneNodePrivate::BoneNodePrivate(BoneNode& boneNode, const std::string& name, uint32_t boneID, const utils::Transform& offsetTransform)
    : NodePrivate(boneNode, name)
    , m_boneID(boneID)
    , m_offsetTransform(offsetTransform)
{
}

BoneNodePrivate::~BoneNodePrivate() = default;

uint32_t& BoneNodePrivate::boneID()
{
    return m_boneID;
}

utils::Transform& BoneNodePrivate::offsetTransform()
{
    return m_offsetTransform;
}



}
}
