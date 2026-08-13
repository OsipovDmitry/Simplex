#ifndef CORE_BLOOMPRIVATE_H
#define CORE_BLOOMPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class BloomPrivate
{
public:
    BloomPrivate(CameraNode&);
    ~BloomPrivate();

    CameraNode& cameraNode();

    bool& isEnabled();
    float& contribution();
    uint32_t& passesCount();

private:
    CameraNode& m_cameraNode;

    bool m_isEnabled = false;
    float m_contribution = .02f;
    uint32_t m_passesCount = 4u;
};

} // namespace core
} // namespace simplex

#endif // CORE_BLOOMPRIVATE_H
