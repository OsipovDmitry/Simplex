#ifndef GRAPHICS_GLFW_FORWARDDECL_H
#define GRAPHICS_GLFW_FORWARDDECL_H

#include <list>
#include <memory>

namespace simplex
{
namespace graphics_glfw
{

class GLFWWidget;
using GLFWWidgetShareGroup = std::list<std::weak_ptr<GLFWWidget>>;

}
}

#endif // GRAPHICS_GLFW_FORWARDDECL_H
