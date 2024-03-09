#include <utils/glm/gtc/matrix_transform.hpp>
#include <utils/clipspace.h>
#include <utils/range.h>

namespace simplex
{
namespace utils
{

AbstractClipSpace::~AbstractClipSpace()
{}

std::shared_ptr<OrthoClipSpace> AbstractClipSpace::asOrthoClipSpace()
{
    return nullptr;
}

std::shared_ptr<const OrthoClipSpace> AbstractClipSpace::asOrthoClipSpace() const
{
    return nullptr;
}

std::shared_ptr<PerspectiveClipSpace> AbstractClipSpace::asPerspectiveClipSpace()
{
    return nullptr;
}

std::shared_ptr<const PerspectiveClipSpace> AbstractClipSpace::asPerspectiveClipSpace() const
{
    return nullptr;
}

OrthoClipSpace::OrthoClipSpace()
{}

std::shared_ptr<OrthoClipSpace> OrthoClipSpace::asOrthoClipSpace()
{
    return std::dynamic_pointer_cast<OrthoClipSpace>(shared_from_this());
}

std::shared_ptr<const OrthoClipSpace> OrthoClipSpace::asOrthoClipSpace() const
{

    return const_cast<OrthoClipSpace*>(this)->asOrthoClipSpace();
}

glm::mat4 OrthoClipSpace::projectionMatrix(float aspect, const Range &zRange) const
{
    return glm::ortho(-aspect, aspect, -1.f, 1.f, zRange.nearValue(), zRange.farValue());
}

PerspectiveClipSpace::PerspectiveClipSpace(float fov)
{
    setFov(fov);
}

float PerspectiveClipSpace::fov() const
{
    return m_fov;
}

void PerspectiveClipSpace::setFov(float fov)
{
    m_fov = fov;
}

std::shared_ptr<PerspectiveClipSpace> PerspectiveClipSpace::asPerspectiveClipSpace()
{
    return std::dynamic_pointer_cast<PerspectiveClipSpace>(shared_from_this());
}

std::shared_ptr<const PerspectiveClipSpace> PerspectiveClipSpace::asPerspectiveClipSpace() const
{
    return const_cast<PerspectiveClipSpace*>(this)->asPerspectiveClipSpace();
}

glm::mat4 PerspectiveClipSpace::projectionMatrix(float aspect, const Range &zRange) const
{
    return glm::perspective(m_fov, aspect, zRange.nearValue(), zRange.farValue());
}


} // namespace
} // namespace
