#ifndef CORE_IRENDERWIDGET_H
#define CORE_IRENDERWIDGET_H

#include <core/inamedobject.h>
#include <core/forwarddecl.h>

#include <utils/forwarddecl.h>
#include <utils/glm/vec2.hpp>

namespace simplex
{
namespace core
{

class IRenderWidget : public INamedObject
{
public:
    ~IRenderWidget() override = default;

    virtual std::string title() const = 0;
    virtual void setTitle(const std::string&) = 0;

    virtual void setIcon(const std::shared_ptr<utils::Image>&) = 0;

    virtual glm::uvec2 position() const = 0;
    virtual void setPosition(const glm::uvec2&) = 0;

    virtual glm::uvec2 size() const = 0;
    virtual void setSize(const glm::uvec2&) = 0;

    virtual void minimize() = 0;
    virtual void maximize() = 0;
    virtual void restore() = 0;

    virtual void show(bool = true) = 0;
    virtual void hide() = 0;

    virtual void setFocus() = 0;

    virtual void swapBuffers() = 0;
};

}
}

#endif // CORE_IRENDERWIDGET_H
