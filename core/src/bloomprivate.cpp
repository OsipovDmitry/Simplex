#include "bloomprivate.h"

namespace simplex
{
namespace core
{

BloomPrivate::BloomPrivate(CameraNode& cameraNode)
    : m_cameraNode(cameraNode)
{
}

BloomPrivate::~BloomPrivate() = default;

CameraNode& BloomPrivate::cameraNode()
{
    return m_cameraNode;
}

bool& BloomPrivate::isEnabled()
{
    return m_isEnabled;
}

float& BloomPrivate::contribution()
{
    return m_contribution;
}

uint32_t& BloomPrivate::passesCount()
{
    return m_passesCount;
}

float& BloomPrivate::upSamplePassBlurRadius()
{
    return m_upSamplePassBlurRadius;
}

} // namespace core
} // namespace simplex
