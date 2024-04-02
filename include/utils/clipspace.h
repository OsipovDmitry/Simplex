#ifndef UTILS_CLIPSPACE_H
#define UTILS_CLIPSPACE_H

#include <memory>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/glm/mat4x4.hpp>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT AbstractClipSpace : public std::enable_shared_from_this<AbstractClipSpace>
{
public:
    virtual ~AbstractClipSpace();

    virtual std::shared_ptr<OrthoClipSpace> asOrthoClipSpace();
    virtual std::shared_ptr<const OrthoClipSpace> asOrthoClipSpace() const;

    virtual std::shared_ptr<PerspectiveClipSpace> asPerspectiveClipSpace();
    virtual std::shared_ptr<const PerspectiveClipSpace> asPerspectiveClipSpace() const;

    virtual glm::mat4 projectionMatrix(float aspect, const utils::Range &zRange) const = 0;
};

class UTILS_SHARED_EXPORT OrthoClipSpace : public AbstractClipSpace
{
public:
    OrthoClipSpace(float height);

    float height() const;
    void setHeight(float);

    std::shared_ptr<OrthoClipSpace> asOrthoClipSpace() override;
    std::shared_ptr<const OrthoClipSpace> asOrthoClipSpace() const override;

    glm::mat4 projectionMatrix(float aspect, const utils::Range &zRange) const override;

protected:
    float m_height;
};

class UTILS_SHARED_EXPORT PerspectiveClipSpace : public AbstractClipSpace
{
public:
    PerspectiveClipSpace(float fov);

    float fov() const;
    void setFov(float);

    std::shared_ptr<PerspectiveClipSpace> asPerspectiveClipSpace() override;
    std::shared_ptr<const PerspectiveClipSpace> asPerspectiveClipSpace() const override;

    glm::mat4 projectionMatrix(float aspect, const utils::Range &zRange) const override;

protected:
    float m_fov;
};

} // namespace
} // namespace

#endif // UTILS_CLIPSPACE_H
