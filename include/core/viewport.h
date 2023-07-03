#ifndef CORE_VIEWPORT_H
#define CORE_VIEWPORT_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/enumclass.h>
#include <utils/glm/vec4.hpp>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class ViewportPrivate;
class CORE_SHARED_EXPORT Viewport
{
    NONCOPYBLE(Viewport)
public:
    ENUMCLASS(SizePolicy, uint32_t,
              Defined,
              Screen,
              HalfScreen,
              QuarterScreen)

    Viewport();
    ~Viewport();

    SizePolicy sizePolicy() const;
    void setSizePolicy(SizePolicy);

    const glm::uvec4 &size() const;
    void setSize(const glm::uvec4&);

private:
    std::unique_ptr<ViewportPrivate> m_;
};

}
}

#endif // CORE_VIEWPORT_H
